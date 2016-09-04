// All right! Yeah

#include "Rubik.h"
#include "RubiksBlock_Standart_3Side.h"


ARubiksBlock_Standart_3Side::ARubiksBlock_Standart_3Side(const class FObjectInitializer& OI)
	: Super(OI)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshObjFinder(TEXT("StaticMesh'/Game/Cube/3Board/3Board_LowPoly.3Board_LowPoly'"));

	UStaticMeshComponent* smComponent = OI.CreateDefaultSubobject < UStaticMeshComponent >(this, MakeUniqueObjectName(this, AActor::StaticClass(), "Block"));
	smComponent->SetStaticMesh(meshObjFinder.Object);
	smComponent->SetupAttachment(RootComponent);
	smComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
}
