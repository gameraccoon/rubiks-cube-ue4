#include "CommandHistory.h"

namespace GameBase
{
	CommandHistory::CommandHistory()
		: currentCommand(nullptr)
	{

	}

	bool CommandHistory::IsOnHead() const
	{
		return !currentCommand || currentCommand->GetNextNode() == nullptr;
	}

	bool CommandHistory::IsOnTail() const
	{
		return !currentCommand || currentCommand->GetPrevNode() == nullptr;
	}

	bool CommandHistory::IsEmpty() const
	{
		return !currentCommand;
	}

	Command::Ptr CommandHistory::GetCurrentCommand()
	{
		return currentCommand ? currentCommand->GetValue() : GameBase::Command::Ptr(nullptr);
	}

	Command::Ptr CommandHistory::MoveForward()
	{
		if (IsOnHead()) {
			return GameBase::Command::Ptr(nullptr);
		} else {
			currentCommand = currentCommand->GetNextNode();
			return GetCurrentCommand();
		}
	}

	Command::Ptr CommandHistory::MoveBackward()
	{
		if (IsOnTail()) {
			return GameBase::Command::Ptr(nullptr);
		}
		else {
			currentCommand = currentCommand->GetPrevNode();
			return GetCurrentCommand();
		}
	}

	void CommandHistory::AddCommand(Command::Ref command)
	{
		FError::Throwf(TEXT("CommandHistory::AddCommand need implementation"));
	}

} // namespace GameBase
