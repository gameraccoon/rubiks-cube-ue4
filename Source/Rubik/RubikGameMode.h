// All right! Yeah

#pragma once

//#include "GameFramework/GameMode.h

#include "RubicsCube.h"

#include "RubikGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RUBIK_API ARubikGameMode : public AGameMode
{
	GENERATED_BODY()
	
	virtual void PostLogin(APlayerController* newPlayer) override;
	
};
