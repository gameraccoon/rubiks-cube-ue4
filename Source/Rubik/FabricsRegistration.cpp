#include "Rubik.h"
#include "FabricsRegistration.h"
#include "Base/CommandFactory.h"
#include "Rubik/RCRotationCommand.h"

namespace GameBase
{
	static bool IsRegistred = false;

	void RegisterCommands()
	{
		RC::RotationCommand::RegisterInFabric();
	}

	void RegisterAll()
	{
		if (IsRegistred)
		{
			return;
		}

		IsRegistred = true;
		RegisterCommands();
	}

} // namespace GameBase
