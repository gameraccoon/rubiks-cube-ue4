#include "Rubik.h"
#include "Base/CommandHistory.h"
#include "Base/CommandFactory.h"

namespace GameBase
{
	CommandHistory::CommandHistory()
		: Current(0)
		, Reciever(nullptr)
	{
	}

	CommandHistory::CommandHistory(AActor* reciever)
		: Current(0)
		, Reciever(reciever)
	{
	}

	bool CommandHistory::IsOnHead() const
	{
		return Current == Commands.Num();
	}

	bool CommandHistory::IsOnTail() const
	{
		return Current == 0;
	}

	bool CommandHistory::IsEmpty() const
	{
		return Commands.Num() == 0;
	}

	Command::Ptr CommandHistory::GetNextCommand()
	{
		return (IsOnHead() || IsEmpty()) ? GameBase::Command::Ptr(nullptr) : Commands[Current];
	}

	Command::Ptr CommandHistory::GetPrevCommand()
	{
		return (IsOnTail() || IsEmpty()) ? GameBase::Command::Ptr(nullptr) : Commands[Current-1];
	}

	void CommandHistory::MoveForward()
	{
		if (!IsOnHead()) {
			++Current;
		} else {
			UE_LOG(LogicalError, Error, TEXT("Move forvard from the head of the history"));
		}
	}

	void CommandHistory::MoveBackward()
	{
		if (!IsOnTail()) {
			--Current;
		}
		else {
			UE_LOG(LogicalError, Error, TEXT("Move backward from the tail of the history"));
		}
	}

	void CommandHistory::AddCommand(Command::Ptr command)
	{
		// if we have some commands after the current then remove them
		ClearNextCommands();

		Commands.Push(command);
		++Current;
	}

	void CommandHistory::ClearNextCommands()
	{
		Commands.SetNum(Current);
	}

	TSharedPtr<FJsonObject> CommandHistory::ToJson() const
	{
		TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());

		TArray<TSharedPtr<FJsonValue>> serializedCommands;

		for (const auto& element : Commands)
		{
			TSharedPtr<FJsonObject> jsonObject = element->ToJson();
			serializedCommands.Add(MakeShareable(new FJsonValueObject(jsonObject)));
		}

		result->SetArrayField("commands", serializedCommands);
		result->SetNumberField("activeCommandIdx", Current);

		return result;
	}

	void CommandHistory::LoadFromJson(const TSharedPtr<FJsonObject> serialized)
	{
		TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());

		TArray<TSharedPtr<FJsonValue>> serializedCommands = serialized->GetArrayField("commands");

		Commands.Reserve(serializedCommands.Num());

		for (const auto& element : serializedCommands)
		{
			Command::Ptr command = CommandFactory::Get().CreateFromJson(element->AsObject());
			if (command.IsValid())
			{
				Commands.Push(command);
				command->SetTarget(Reciever);
				command->Execute();
			}
			else
			{
				UE_LOG(LogicalError, Error, TEXT("Problem with deserializing commands"));
				Commands.Empty();
				Current = 0;
				return;
			}
		}

		Current = serialized->GetIntegerField("activeCommandIdx");
	}

} // namespace GameBase
