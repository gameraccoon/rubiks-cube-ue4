// All right! Yeah

#include "Rubik.h"
#include "RubiksSide_Standart.h"


// Sets default values
ARubiksSide_Standart::ARubiksSide_Standart(const class FObjectInitializer& OI)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = OI.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComponent"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshObjFinder(TEXT("StaticMesh'/Game/Cube/Side/Side_low.Side_low'"));

	UStaticMeshComponent* smComponent = OI.CreateDefaultSubobject < UStaticMeshComponent >(this, MakeUniqueObjectName(this, AActor::StaticClass(), "Side"));
	smComponent->SetStaticMesh(meshObjFinder.Object);
	smComponent->AttachParent = RootComponent;
	smComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
}

// Called when the game starts or when spawned
void ARubiksSide_Standart::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARubiksSide_Standart::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}


void ARubiksSide_Standart::SetDirections(const TArray<RC::MovementDirection>& directions)
{
	Directions = directions;
}

TArray<RC::MovementDirection> ARubiksSide_Standart::GetDirections() const
{
	return Directions;
}
