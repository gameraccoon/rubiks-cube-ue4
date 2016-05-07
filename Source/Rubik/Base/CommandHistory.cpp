#include "Rubik.h"
#include "Base/CommandHistory.h"
#include "Base/CommandFactory.h"

namespace GameBase
{
	CommandHistory::CommandHistory()
		: current(0)
		, reciever(nullptr)
	{
	}

	CommandHistory::CommandHistory(AActor* reciever)
		: current(0)
		, reciever(reciever)
	{
	}

	bool CommandHistory::IsOnHead() const
	{
		return current == commands.Num();
	}

	bool CommandHistory::IsOnTail() const
	{
		return current == 0;
	}

	bool CommandHistory::IsEmpty() const
	{
		return commands.Num() == 0;
	}

	Command::Ptr CommandHistory::GetNextCommand()
	{
		return (IsOnHead() || IsEmpty()) ? GameBase::Command::Ptr(nullptr) : commands[current];
	}

	Command::Ptr CommandHistory::GetPrevCommand()
	{
		return (IsOnTail() || IsEmpty()) ? GameBase::Command::Ptr(nullptr) : commands[current-1];
	}

	void CommandHistory::MoveForward()
	{
		if (!IsOnHead()) {
			++current;
		} else {
			FError::Throwf(TEXT("Move forvard from the head of the history"));
		}
	}

	void CommandHistory::MoveBackward()
	{
		if (!IsOnTail()) {
			--current;
		}
		else {
			FError::Throwf(TEXT("Move backward from the tail of the history"));
		}
	}

	void CommandHistory::AddCommand(Command::Ptr command)
	{
		// if we have some commands after the current then remove them
		ClearNextCommands();

		commands.Push(command);
		++current;
	}

	void CommandHistory::ClearNextCommands()
	{
		commands.SetNum(current);
	}

	TSharedPtr<FJsonObject> CommandHistory::ToJson() const
	{
		TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());

		TArray<TSharedPtr<FJsonValue>> serializedCommands;

		for (const auto& element : commands)
		{
			TSharedPtr<FJsonObject> jsonObject = element->ToJson();
			serializedCommands.Add(MakeShareable(new FJsonValueObject(jsonObject)));
		}

		result->SetArrayField("commands", serializedCommands);
		result->SetNumberField("activeCommandIdx", current);

		return result;
	}

	void CommandHistory::LoadFromJson(const TSharedPtr<FJsonObject> serialized)
	{
		TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());

		TArray<TSharedPtr<FJsonValue>> serializedCommands = serialized->GetArrayField("commands");

		commands.Reserve(serializedCommands.Num());

		for (const auto& element : serializedCommands)
		{
			Command::Ptr command = CommandFactory::Get().CreateFromJson(element->AsObject());
			if (command.IsValid())
			{
				commands.Push(command);
				command->SetTarget(reciever);
				command->Execute();
			}
			else
			{
				FError::Throwf(TEXT("Problem with deserializing commands"));
				commands.Empty();
				current = 0;
				return;
			}
		}

		current = serialized->GetIntegerField("activeCommandIdx");
	}

} // namespace GameBase
