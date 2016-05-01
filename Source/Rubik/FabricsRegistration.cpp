#include "Rubik.h"
#include "FabricsRegistration.h"
#include "Base/CommandFactory.h"
#include "Rubik/RCRotationCommand.h"

namespace GameBase
{
	void RegisterCommands()
	{
		RC::RotationCommand::RegisterInFabric();
	}

	void RegisterAll()
	{
		RegisterCommands();
	}

} // namespace GameBase
