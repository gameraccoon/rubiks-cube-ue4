#pragma once

#include "Command.h"

namespace GameBase
{
	class CommandHistory
	{
	public:
		CommandHistory();
		CommandHistory(AActor* reciever);
		/// return true if there is not command in front of the current state
		bool IsOnHead() const;
		/// retrun true if there is no command in back of the current state
		bool IsOnTail() const;
		/// retrun true if history is empty
		bool IsEmpty() const;
		/// return the next command or nullptr if history is empty
		Command::Ptr GetNextCommand();
		/// return the previous command or nullptr if history is empty
		Command::Ptr GetPrevCommand();
		/// swith current history state to the next command
		void MoveForward();
		/// swith current history state to the previous command
		void MoveBackward();
		/// add new command in front of the current state (and remove all commands that were after the current state)
		void AddCommand(Command::Ptr command);
		/// remove all commands in front of the current
		void ClearNextCommands();

		TSharedPtr<FJsonObject> ToJson() const;
		void LoadFromJson(const TSharedPtr<FJsonObject> serialized);

	private:
		AActor* Reciever;
		TArray<Command::Ptr> Commands;
		int Current;
	};
} // namespace GameBase
