// All right! Yeah

#pragma once

#include "MultiTouchHelper.h"
#include "GameFramework/PlayerController.h"
#include "RCRotationAxis.h"
#include "RubiksPlayerController.generated.h"

class ARubicsCube;
class ARubiksPlayerPawn;

namespace RC
{
	struct ScreenMovementDiraction
	{
		ScreenMovementDiraction()
			: direction(0.0f, 0.0f)
			, axis()
			, layerIndex(-1)
		{
		}

		ScreenMovementDiraction(const FVector2D& _direction, RC::RotationAxis _axis, int _layerIndex)
			: direction(_direction)
			, axis(_axis)
			, layerIndex(_layerIndex)
		{
		}

		FVector2D direction;
		RC::RotationAxis axis;
		int layerIndex;
	};
}

/**
 * 
 */
UCLASS()
class RUBIK_API ARubiksPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual bool InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad) override;
	virtual bool InputTouch(uint32 Handle, ETouchType::Type Type, const FVector2D& TouchLocation, FDateTime DeviceTimestamp, uint32 TouchpadIndex) override;
	virtual bool InputAxis(FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad) override;
	virtual bool InputMotion(const FVector& Tilt, const FVector& RotationRate, const FVector& Gravity, const FVector& Acceleration) override;

private:
	void RotateCube(const FRotator& rotation);
	void SetCameraRotation(const FRotator& rotation);
	void FindPlayerPawn();

	void CheckAllComponents();
	void TryToAttachCubeToPawn();
	bool IsAllComponentsReady() const;

	bool ActorIsCube(const AActor* actor) const;

	void StartRotateCubePart(AActor* cubePart, const FVector2D& TouchLocation);
	void TryToRotateCubePart(const FVector2D& TouchLocation);

	AActor* GetActorUnderPoint(const FVector2D& point) const;

	// Tap or Click
	void ProcessTap(AActor* tappedActor);

private:
	/// Cube that we control at this time
	ARubicsCube* mainCube;
	ARubiksPlayerPawn* playerPawn;
	Utils::MultitouchHelper Multitouch;

	// for optimization
	uint32 LastTouchesMax = 0;
	uint32 ThisTouchesMax = 0;

	int RotationsLockIndex = -1;
	TArray<RC::ScreenMovementDiraction> CurrentSideDirections;
	FVector2D MovementTouchStartLocation;
	bool RotationCompleted = false;

	AActor* SwipeStartActor = nullptr;
};
