#pragma once

#include "Command.h"

namespace GameBase
{
	class CommandHistory
	{
	public:
		CommandHistory();
		/// return true if there is not command in front of the current state
		bool IsOnHead() const;
		/// retrun true if there is no command in back of the current state
		bool IsOnTail() const;
		/// retrun true if history is empty
		bool IsEmpty() const;
		/// return current command or nullptr if history is empty
		Command::Ptr GetCurrentCommand();
		/// return the next command and swith current history state or return nullptr if is on head
		Command::Ptr MoveForward();
		/// return the prevoius command and swith current history state or return nullptr if is on tail
		Command::Ptr MoveBackward();
		/// add new command in front of current (and remove all commands that were after current)
		void AddCommand(Command::Ref command);

	private:
		TDoubleLinkedList<Command::Ref> commands;
		TDoubleLinkedList<Command::Ref>::TDoubleLinkedListNode* currentCommand;
	};
} // namespace GameBase
