// All right! Yeah

#include "Rubik.h"
#include "RubiksBlock_Standart_1Side.h"


ARubiksBlock_Standart_1Side::ARubiksBlock_Standart_1Side(const class FObjectInitializer& OI)
	: Super(OI)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshObjFinder(TEXT("StaticMesh'/Game/Cube/1Board/1Board_LowPoly.1Board_LowPoly'"));

	UStaticMeshComponent* smComponent = OI.CreateDefaultSubobject < UStaticMeshComponent >(this, MakeUniqueObjectName(this, AActor::StaticClass(), "Block"));
	smComponent->SetStaticMesh(meshObjFinder.Object);
	smComponent->SetupAttachment(RootComponent);
	smComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
}
