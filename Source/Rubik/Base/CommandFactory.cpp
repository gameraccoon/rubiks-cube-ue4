#include "Rubik.h"
#include "Base/CommandHistory.h"
#include "CommandFactory.h"

namespace GameBase
{
	CommandFactory & CommandFactory::Get()
	{
		static CommandFactory instance;
		return instance;
	}

	void CommandFactory::RegisterCommand(const FString & commandId, CreateCommandCallback createFn)
	{
		callbacks.Add(commandId, createFn);
	}

	Command::Ptr CommandFactory::CreateFromJson(TSharedPtr<FJsonObject> serialized)
	{
		auto callback = callbacks.Find(serialized->GetStringField("commandId"));

		if (callback)
		{
			FError::Throwf(TEXT("Unimplemented command"));
			return Command::Ptr(nullptr);
		}

		return (*callback)(serialized);
	}

} // namespace GameBase
