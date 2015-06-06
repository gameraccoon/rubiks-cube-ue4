// All right! Yeah

#include "Rubik.h"
#include "RubiksPlayerController.h"
#include "RubicsCube.h"
#include "RubiksPlayerPawn.h"
#include "RubiksSide_Standart.h"
#include "RubikPart.h"

static const float MOVEMENT_SWIPE_TOLERANCE = 10.0f;

bool ARubiksPlayerController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	if (Key == FKey("MouseScrollUp"))
	{
		CheckAllComponents();

		if (IsAllComponentsReady())
		{
			playerPawn->CameraOffset /= 1.1;
		}
	}
	else if (Key == FKey("MouseScrollDown"))
	{
		CheckAllComponents();

		if (IsAllComponentsReady())
		{
			playerPawn->CameraOffset *= 1.1;
		}
	}

	if (Key == FKey("LeftMouseButton") && EventType == EInputEvent::IE_Pressed)
	{
		FVector2D mousePos;
		GetMousePosition(mousePos.X, mousePos.Y);
		StartRotateCubePart(mousePos);
		SwipeStartActor = GetActorUnderPoint(mousePos);
	}
	else if (Key == FKey("LeftMouseButton") && EventType == EInputEvent::IE_Released)
	{
		if (SwipeStartActor != nullptr)
		{
			ProcessTap(SwipeStartActor);
			SwipeStartActor = nullptr;
		}
	}

	Super::InputKey(Key, EventType, AmountDepressed, bGamepad);

	return true;
}

bool ARubiksPlayerController::InputTouch(uint32 Handle, ETouchType::Type Type, const FVector2D& TouchLocation, FDateTime DeviceTimestamp, uint32 TouchpadIndex)
{
	ThisTouchesMax = 0; // optimization

	if (RotationsLockIndex != -1 && Handle != RotationsLockIndex)
	{
		if (Type == ETouchType::Ended)
		{
			Multitouch.RemoveTouch(Handle);
		}

		return true;
	}

	if (Type == ETouchType::Began)
	{
		if (IsCubeUnderPoint(TouchLocation))
		{
			RotationsLockIndex = Handle;
			StartRotateCubePart(TouchLocation);
			SwipeStartActor = GetActorUnderPoint(TouchLocation);
			return true;
		}

		Multitouch.AddTouch(Handle, TouchLocation);
		if (ThisTouchesMax < Handle) { ThisTouchesMax = Handle; } // optimization
	}
	else if (Type == ETouchType::Moved)
	{
		if (RotationsLockIndex == Handle)
		{
			TryToRotateCubePart(TouchLocation);
		}

		if (SwipeStartActor != nullptr)
		{
			AActor* actorUnderPoint = GetActorUnderPoint(TouchLocation);
			if (actorUnderPoint != SwipeStartActor)
			{
				SwipeStartActor = nullptr;
			}
		}

		Multitouch.MoveTouch(Handle, TouchLocation);
		if (ThisTouchesMax < Handle) { ThisTouchesMax = Handle; } // optimization
	}
	else if (Type == ETouchType::Ended)
	{
		Multitouch.RemoveTouch(Handle);

		if (LastTouchesMax == Handle) { LastTouchesMax = ThisTouchesMax; } // optimization

		if (Handle == RotationsLockIndex)
		{
			RotationsLockIndex = -1;
		}

		if (SwipeStartActor != nullptr)
		{
			ProcessTap(SwipeStartActor);
			SwipeStartActor = nullptr;
		}
	}

	if (LastTouchesMax < ThisTouchesMax) { LastTouchesMax = ThisTouchesMax; } // optimization

	if (Handle < LastTouchesMax) // optimization
	{
		return true;
	}

	if (Multitouch.GetTouchesCount() == 1)
	{
		FVector2D movement = Multitouch.GetMoveVector();
		RotateCube(FRotator(movement.Y, -movement.X, 0.0f));
	}
	else
	{
		float spin = Multitouch.GetSpinAngle();
		RotateCube(FRotator(0.0f, 0.0f, -spin));
	}

	return true;
}

bool ARubiksPlayerController::InputAxis(FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	if (IsInputKeyDown(FKey("RightMouseButton")))
	{
		FVector2D mouseDelta;
		GetInputMouseDelta(mouseDelta.X, mouseDelta.Y);

		if (IsInputKeyDown(FKey("LeftShift")))
		{
			RotateCube(FRotator(0.0f, 0.0f, mouseDelta.X));
		}
		else
		{
			RotateCube(FRotator(-mouseDelta.Y, -mouseDelta.X, 0.0f));
		}
	}
	else if (IsInputKeyDown(FKey("LeftMouseButton")))
	{
		FVector2D mousePos;
		GetMousePosition(mousePos.X, mousePos.Y);
		TryToRotateCubePart(mousePos);

		if (SwipeStartActor != nullptr)
		{
			AActor* actorUnderPoint = GetActorUnderPoint(mousePos);
			if (actorUnderPoint != SwipeStartActor)
			{
				SwipeStartActor = nullptr;
			}
		}
	}

	Super::InputAxis(Key, Delta, DeltaTime, NumSamples, bGamepad);
	return true;
}

bool ARubiksPlayerController::InputMotion(const FVector& Tilt, const FVector& RotationRate, const FVector& Gravity, const FVector& Acceleration)
{
	//SetCameraRotation(Gravity.Rotation());
	return true;
}

void ARubiksPlayerController::RotateCube(const FRotator& rotation)
{
	CheckAllComponents();

	if (IsAllComponentsReady())
	{
		FQuat fullRotation = FQuat(rotation) * FQuat(mainCube->GetActorRotation());
		FQuat partialRotation = FQuat::FastLerp(mainCube->GetActorRotation().Quaternion(), fullRotation, 0.3);
		mainCube->SetActorRotation(partialRotation.Rotator());
		mainCube->ScheduleUpdateParts();
	}
}

void ARubiksPlayerController::SetCameraRotation(const FRotator& rotation)
{
	CheckAllComponents();

	if (IsAllComponentsReady())
	{
		playerPawn->CameraYaw = rotation.Yaw;
		playerPawn->CameraPitch = -rotation.Pitch;
		playerPawn->CameraRoll = rotation.Roll;
	}
}

void ARubiksPlayerController::FindPlayerPawn()
{
	playerPawn = dynamic_cast<ARubiksPlayerPawn*>(GetPawn());

	TryToAttachCubeToPawn();
}

void ARubiksPlayerController::CheckAllComponents()
{
	if (!playerPawn)
	{
		FindPlayerPawn();
	}

	if (playerPawn && !mainCube)
	{
		if (!playerPawn->Cube)
		{
			playerPawn->FindCube();
		}

		mainCube = playerPawn->Cube;
	}
}

void ARubiksPlayerController::TryToAttachCubeToPawn()
{
	if (mainCube && playerPawn)
	{
		playerPawn->Cube = mainCube;
	}
}

bool ARubiksPlayerController::IsAllComponentsReady() const
{
	return mainCube && playerPawn;
}

bool ARubiksPlayerController::IsCubeUnderPoint(const FVector2D& point) const
{
	AActor * actor = GetActorUnderPoint(point);

	if (actor && actor->IsA(ARubikPart::StaticClass()))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ARubiksPlayerController::StartRotateCubePart(const FVector2D& TouchLocation)
{
	AActor * actor = GetActorUnderPoint(TouchLocation);

	if (actor && actor->IsA(ARubiksSide_Standart::StaticClass()))
	{
		CheckAllComponents();

		FVector2D actorScreenPos;
		ProjectWorldLocationToScreen(actor->GetActorLocation(), actorScreenPos);

		if (mainCube)
		{
			CurrentSideDirections.Empty();
			TArray<RC::MovementDirection> directions = dynamic_cast<ARubiksSide_Standart*>(actor)->GetDirections();
			for (const auto& direction : directions)
			{
				FVector2D screenLocation;
				ProjectWorldLocationToScreen(actor->GetActorLocation() + FQuat(mainCube->GetActorRotation()).RotateVector(direction.direction), screenLocation);
				CurrentSideDirections.Push(RC::ScreenMovementDiraction(screenLocation - actorScreenPos, direction.axis, direction.layerIndex));
			}
			RotationCompleted = false;
		}

		MovementTouchStartLocation = TouchLocation;
	}
}

void ARubiksPlayerController::TryToRotateCubePart(const FVector2D& TouchLocation)
{
	if (mainCube && !RotationCompleted)
	{
		float minimalAngle = 1000.0f;
		RC::ScreenMovementDiraction bestDirection;
		FVector2D movementDelta = TouchLocation - MovementTouchStartLocation;
		float movementLength = -1.0f;

		for (const auto& direction : CurrentSideDirections)
		{
			float angle = FMath::Atan2(direction.direction.X, direction.direction.Y) - FMath::Atan2(movementDelta.X, movementDelta.Y);
			if (angle > PI) { angle -= 2 * PI; }
			if (angle < -PI) { angle += 2 * PI; }
			if (FMath::Abs(angle) < minimalAngle)
			{
				minimalAngle = FMath::Abs(angle);
				bestDirection = direction;
				movementLength = direction.direction.Size();
			}
		}

		if (movementLength > 0.0f && movementDelta.Size() > (movementLength * MOVEMENT_SWIPE_TOLERANCE))
		{
			mainCube->AddRotation(bestDirection.axis, bestDirection.layerIndex);
			RotationCompleted = true;
		}
	}
}

AActor* ARubiksPlayerController::GetActorUnderPoint(const FVector2D& point) const
{
	FVector location, direction;
	DeprojectScreenPositionToWorld(point.X, point.Y, location, direction);

	FCollisionQueryParams TraceParams(FName(TEXT("")), true, this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = false;

	FHitResult hit(ForceInit);
	float endOffset = 500;
	FVector target = location + (direction * endOffset);
	GetWorld()->LineTraceSingle(hit, location, target, ECC_GameTraceChannel1, TraceParams);
	return hit.GetActor();
}

void ARubiksPlayerController::ProcessTap(AActor* tappedActor)
{
	if (tappedActor->GetName() == "Undo")
	{
		CheckAllComponents();

		if (mainCube)
		{
			mainCube->UndoRotation();
		}
	}
	else if (tappedActor->GetName() == "Redo")
	{
		CheckAllComponents();

		if (mainCube)
		{
			mainCube->RedoRotation();
		}
	}
}
