#pragma once

#include "Command.h"

class CommandFactory
{
public:
	typedef Command::Ptr (*CreateCommandCallback)(const TSharedPtr<FJsonObject>);

public:
	static CommandFactory& Get();

	void RegisterCommand(const FString& CommandId, CreateCommandCallback CreateFn);

	Command::Ptr CreateFromSerialized(const FString& Serialized);

private:
	typedef TMap<FString, CreateCommandCallback> CallbackMap;

private:
	CallbackMap Callbacks;

private:
	CommandFactory() = default;
	~CommandFactory() = default;
	CommandFactory(const CommandFactory&) = delete;
	void operator=(const CommandFactory&) = delete;
};
