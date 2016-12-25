// All right! Yeah

#include "Rubik.h"
#include "RubiksBlock_Standart.h"


ARubiksBlock_Standart::ARubiksBlock_Standart(const class FObjectInitializer& OI)
	: Super(OI)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshObjFinder(TEXT("StaticMesh'/Game/Cube/Block/Block_BlackPlasic.Block_BlackPlasic'"));

	UStaticMeshComponent* smComponent = OI.CreateDefaultSubobject < UStaticMeshComponent >(this, MakeUniqueObjectName(this, AActor::StaticClass(), "Block"));
	smComponent->SetStaticMesh(meshObjFinder.Object);
	smComponent->SetupAttachment(RootComponent);
	smComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
}
