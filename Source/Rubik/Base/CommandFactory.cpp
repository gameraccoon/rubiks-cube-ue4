#include "Rubik.h"
#include "Base/CommandHistory.h"
#include "CommandFactory.h"


CommandFactory & CommandFactory::Get()
{
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

	auto Callback = Callbacks.Find(JsonObject->GetStringField("commandId"));
	
	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		if (Callback)
		{
			return (*Callback)(JsonObject);
		}
	}

	return Command::Ptr(nullptr);
}
