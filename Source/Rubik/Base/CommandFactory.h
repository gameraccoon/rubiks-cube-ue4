#pragma once

#include "Command.h"

namespace GameBase
{
	class CommandFactory
	{
		public:typedef Command::Ptr (*CreateCommandCallback)(TSharedPtr<FJsonObject>);

	public:
		static CommandFactory& Get();

		void RegisterCommand(const FString& commandId, CreateCommandCallback createFn);

		Command::Ptr CreateFromJson(TSharedPtr<FJsonObject> serialized);

	private:
		typedef TMap<FString, CreateCommandCallback> CallbackMap;

	private:
		CallbackMap callbacks;

	private:
		CommandFactory() = default;
		~CommandFactory() = default;
		CommandFactory(const CommandFactory&) = delete;
		void operator=(const CommandFactory&) = delete;
	};
} // namespace GameBase
