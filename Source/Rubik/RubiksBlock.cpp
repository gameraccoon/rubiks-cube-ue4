// All right! Yeah

#include "Rubik.h"
#include "RubiksBlock.h"


// Sets default values
ARubiksBlock::ARubiksBlock(const class FObjectInitializer& OI)
	: Super(OI)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = OI.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComponent"));
}

// Called when the game starts or when spawned
void ARubiksBlock::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARubiksBlock::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

