// All right! Yeah

#pragma once

#include "GameFramework/Actor.h"
#include "RubikPart.h"
#include "RubiksBlock.generated.h"

UCLASS()
class RUBIK_API ARubiksBlock : public ARubikPart
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARubiksBlock(const class FObjectInitializer& OI);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
};
