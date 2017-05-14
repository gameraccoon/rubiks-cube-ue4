#include "Rubik.h"
#include "Base/CommandHistory.h"
#include "CommandFactory.h"
#include "FabricsRegistration.h"


CommandFactory& CommandFactory::Get()
{
	GameBase::RegisterAll();
	static CommandFactory Instance;
	return Instance;
}

void CommandFactory::RegisterCommand(const FString& CommandId, CreateCommandCallback CreateFn)
{
	Callbacks.Add(CommandId, CreateFn);
}

Command::Ptr CommandFactory::CreateFromSerialized(const FString& Serialized)
{
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Serialized);
	TSharedPtr<FJsonObject> JsonObject;

	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		if (auto Callback = Callbacks.Find(JsonObject->GetStringField("type")))
		{
			return (*Callback)(JsonObject);
		}
	}

	return Command::Ptr(nullptr);
}
