// All right! Yeah

#pragma once

#include "GameFramework/PlayerController.h"
#include "RubiksPlayerController.generated.h"

class ARubicsCube;
class ARubiksPlayerPawn;

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
	void FindCube();
	void FindPlayerPawn();

	void CheckAllComponents();
	void TryToAttachCubeToPawn();
	bool IsAllComponentsReady() const;

private:
	FVector2D LastTouchPosition;
	/// Cube that we control at this time
	ARubicsCube* mainCube;
	ARubiksPlayerPawn* playerPawn;
};
