// All right! Yeah

#pragma once

#include "GameFramework/Pawn.h"
#include "RubiksPlayerPawn.generated.h"

UCLASS()
class RUBIK_API ARubiksPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARubiksPlayerPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	
public:
	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;
	
public:
	/** Camera pitch and yaw rotations, updated by mouse axis in player input */
	float CameraPitch, CameraYaw;
	AActor* Cube;
	float CameraOffset;

};
