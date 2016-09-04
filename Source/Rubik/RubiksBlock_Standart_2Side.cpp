// All right! Yeah

#include "Rubik.h"
#include "RubiksBlock_Standart_2Side.h"


ARubiksBlock_Standart_2Side::ARubiksBlock_Standart_2Side(const class FObjectInitializer& OI)
	: Super(OI)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshObjFinder(TEXT("StaticMesh'/Game/Cube/2Board/2Board_LowPoly.2Board_LowPoly'"));

	UStaticMeshComponent* smComponent = OI.CreateDefaultSubobject<UStaticMeshComponent>(this, MakeUniqueObjectName(this, AActor::StaticClass(), "Block"));
	smComponent->SetStaticMesh(meshObjFinder.Object);
	smComponent->SetupAttachment(RootComponent);
	smComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
}
