// All right! Yeah

#pragma once

#include "Utils/MultiTouchHelper.h"
#include "GameFramework/PlayerController.h"
#include "RCRotationAxis.h"
#include "RubiksPlayerController.generated.h"

class ARubicsCube;

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
	virtual bool InputTouch(uint32 Handle, ETouchType::Type Type, const FVector2D& TouchLocation, float Force, FDateTime DeviceTimestamp, uint32 TouchpadIndex) override;
	virtual bool InputAxis(FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad) override;
	virtual bool InputMotion(const FVector& Tilt, const FVector& RotationRate, const FVector& Gravity, const FVector& Acceleration) override;

	// Tap or Click
	UFUNCTION(BlueprintImplementableEvent)
	void ProcessTap(AActor* tappedActor);

	UFUNCTION(BlueprintCallable, Category = "Cube")
	ARubicsCube* GetCube();

	UFUNCTION(BlueprintCallable, Category = "Cube")
	void SetCube(ARubicsCube* NewCube);

	UFUNCTION(BlueprintCallable, Category = "Controls")
	void SetRMBSensitivity(float NewSensitivity);

	UFUNCTION(BlueprintCallable, Category = "Controls")
	void SetTouchRotationSensitivity(float NewSensitivity);

	UFUNCTION(BlueprintCallable, Category = "Controls")
	void SetTwoFingersRotationSensitivity(float NewSensitivity);

private:
	bool IsCubeReady() const;

	void RotateCube(const FRotator& rotation);

	bool ActorIsCube(const AActor* actor) const;

	void StartRotateCubePart(AActor* cubePart, const FVector2D& TouchLocation);
	void TryToRotateCubePart(const FVector2D& TouchLocation);

	AActor* GetActorUnderPoint(const FVector2D& point) const;

private:
	/// Cube that we control at the moment
	UPROPERTY()
	ARubicsCube* MainCube = nullptr;

	Utils::MultitouchHelper Multitouch;

	// for optimization
	uint32 LastTouchesMax = 0;
	uint32 ThisTouchesMax = 0;

	int RotationsLockIndex = -1;
	TArray<RC::ScreenMovementDiraction> CurrentSideDirections;
	FVector2D MovementTouchStartLocation;
	bool RotationCompleted = false;

	float RMBSensitivity = 4.0f;
	float TouchRotationSensitivity = 1.0f;
	float TwoFingersRotationSensitivity = 1.0f;

	UPROPERTY()
	AActor* SwipeStartActor = nullptr;
};
