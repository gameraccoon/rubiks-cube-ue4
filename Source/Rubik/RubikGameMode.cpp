// All right! Yeah

#include "Rubik.h"
#include "RubikGameMode.h"

#include "FabricsRegistration.h"


void ARubikGameMode::PostLogin(APlayerController * newPlayer)
{
	GameBase::RegisterAll();

	Super::PostLogin(newPlayer);
}
