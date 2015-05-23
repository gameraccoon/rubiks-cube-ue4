#include "Rubik.h"
#include "Base/CommandHistory.h"

namespace GameBase
{
	CommandHistory::CommandHistory()
	{
		cursor.prew = nullptr;
		cursor.next = nullptr;
	}

	bool CommandHistory::IsOnHead() const
	{
		return !cursor.next;
	}

	bool CommandHistory::IsOnTail() const
	{
		return !cursor.prew;
	}

	bool CommandHistory::IsEmpty() const
	{
		return !cursor.next && !cursor.prew;
	}

	Command::Ptr CommandHistory::GetNextCommand()
	{
		return cursor.next ? cursor.next->GetValue() : GameBase::Command::Ptr(nullptr);
	}

	Command::Ptr CommandHistory::GetPrewCommand()
	{
		return cursor.prew ? cursor.prew->GetValue() : GameBase::Command::Ptr(nullptr);
	}

	void CommandHistory::MoveForward()
	{
		if (!IsOnHead()) {
			cursor.prew = cursor.next;
			cursor.next = cursor.next->GetPrevNode();
		} else {
			FError::Throwf(TEXT("Move forvard from the head of the history"));
		}
	}

	void CommandHistory::MoveBackward()
	{
		if (!IsOnTail()) {
			cursor.next = cursor.prew;
			cursor.prew = cursor.prew->GetNextNode();
		}
		else {
			FError::Throwf(TEXT("Move backward from the tail of the history"));
		}
	}

	void CommandHistory::AddCommand(Command::Ref command)
	{
		// if we have some commands after the current then remove them
		if (!IsOnHead())
		{
			auto headCommand = commands.GetHead();
			while (headCommand != cursor.prew)
			{
				commands.RemoveNode(headCommand);
				headCommand = commands.GetHead();
			}
		}

		commands.AddHead(command);
		cursor.prew = commands.GetHead();
		cursor.next = nullptr;
	}

} // namespace GameBase
