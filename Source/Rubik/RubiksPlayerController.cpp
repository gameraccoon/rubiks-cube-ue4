// All right! Yeah

#include "Rubik.h"
#include "RubiksPlayerController.h"
#include "RubicsCube.h"
#include "RubiksPlayerPawn.h"

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

	Super::InputKey(Key, EventType, AmountDepressed, bGamepad);

	return true;
}

bool ARubiksPlayerController::InputTouch(uint32 Handle, ETouchType::Type Type, const FVector2D& TouchLocation, FDateTime DeviceTimestamp, uint32 TouchpadIndex)
{
	ThisTouchesMax = 0; // optimization

	if (Type == ETouchType::Began)
	{
		Multitouch.AddTouch(Handle, TouchLocation);
		if (ThisTouchesMax < Handle) { ThisTouchesMax = Handle; } // optimization
	}
	else if (Type == ETouchType::Moved)
	{
		Multitouch.MoveTouch(Handle, TouchLocation);
		if (ThisTouchesMax < Handle) { ThisTouchesMax = Handle; } // optimization
	}
	else if (Type == ETouchType::Ended)
	{
		Multitouch.RemoveTouch(Handle);
		if (LastTouchesMax == Handle) { LastTouchesMax = ThisTouchesMax; } // optimization
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

		//float pitch = Multitouch.GetPitchSize();
		//if (pitch > 0.0f)
		//{
		//	playerPawn->CameraOffset *= 1.01;
		//}
		//else if (pitch < 0.0f)
		//{
		//	playerPawn->CameraOffset /= 1.01;
		//}
	}

	return true;
}

bool ARubiksPlayerController::InputAxis(FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	if (IsInputKeyDown(FKey("RightMouseButton")))
	{
		FVector2D mouseDelta;
		GetInputMouseDelta(mouseDelta.X, mouseDelta.Y);
		RotateCube(FRotator(-mouseDelta.Y, -mouseDelta.X, 0.0f));
	}
	else if (IsInputKeyDown(FKey("LeftMouseButton")))
	{
		//FVector2D mousePos;
		//GetMousePosition(mousePos.X, mousePos.Y);
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
	}
}

void ARubiksPlayerController::FindCube()
{
	for (TActorIterator<AActor> AllActorsItr(GetWorld()); AllActorsItr; ++AllActorsItr)
	{
		if (AllActorsItr->GetName() == "MainCube")
		{
			mainCube = dynamic_cast<ARubicsCube*>(*AllActorsItr);
			break;
		}
	}

	TryToAttachCubeToPawn();
}

void ARubiksPlayerController::FindPlayerPawn()
{
	playerPawn = dynamic_cast<ARubiksPlayerPawn*>(GetPawn());

	TryToAttachCubeToPawn();
}

void ARubiksPlayerController::CheckAllComponents()
{
	if (!mainCube)
	{
		FindCube();
	}

	if (!playerPawn)
	{
		FindPlayerPawn();
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
