// All right! Yeah

#include "Rubik.h"
#include "RubiksPlayerPawn.h"


// Sets default values
ARubiksPlayerPawn::ARubiksPlayerPawn()
	: CameraPitch(0)
	, CameraYaw(0)
	, CameraRoll(0)
	, CameraOffset(150.0f)
	, Cube(nullptr)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARubiksPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARubiksPlayerPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void ARubiksPlayerPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void ARubiksPlayerPawn::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	if (!Cube)
	{
		FindCube();

		if (!Cube)
		{
			return;
		}
	}

	const FVector CharacterLocation = Cube->GetActorLocation();
	const FRotationMatrix CameraRotation(FRotator(CameraPitch, CameraYaw, 0.0f));

	// Assume the back of the player is -ve x axis
	const FVector PreTransformedCameraPos(-CameraOffset, 0.0f, 0.0f);
	const FVector CameraLocation = CharacterLocation + CameraRotation.TransformPosition(PreTransformedCameraPos);
	OutResult.Location = CameraLocation;

	// Make Camera look at Target
	OutResult.Rotation = (CharacterLocation - CameraLocation).Rotation();
	OutResult.Rotation.Roll = CameraRoll;
}

void ARubiksPlayerPawn::FindCube()
{
	for (TActorIterator<AActor> AllActorsItr(GetWorld()); AllActorsItr; ++AllActorsItr)
	{
		if (AllActorsItr->GetName() == "MainCube")
		{
			Cube = dynamic_cast<ARubicsCube*>(*AllActorsItr);
			Cube->ScheduleUpdateParts();
		}
	}
}
