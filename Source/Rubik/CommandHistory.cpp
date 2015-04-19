#include "Rubik.h"
#include "Base/CommandHistory.h"

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
		// if we have some commands after the current remove them
		if (!IsEmpty() && !IsOnHead())
		{
			auto headCommand = commands.GetHead();
			while (headCommand != currentCommand)
			{
				commands.RemoveNode(headCommand);
				headCommand = commands.GetHead();
			}
		}

		commands.AddHead(command);
		currentCommand = commands.GetHead();
	}

} // namespace GameBase
