#include "Rubik.h"
#include "Base/CommandHistory.h"
#include "Base/CommandFactory.h"


UCommandHistory::UCommandHistory()
	: Current(0)
	, Reciever(nullptr)
	, FirstChangableCommand(0)
	, IsInited(false)
{
}

UCommandHistory::UCommandHistory(AActor* reciever)
	: Current(0)
	, Reciever(reciever)
	, FirstChangableCommand(0)
	, IsInited(false)
{
}

bool UCommandHistory::IsOnHead() const
{
	if (!IsInited)
	{
		UE_LOG(LogicalError, Error, TEXT("Getting data before initialization"));
	}

	return Current == Commands.Num();
}

bool UCommandHistory::IsOnTail() const
{
	if (!IsInited)
	{
		UE_LOG(LogicalError, Error, TEXT("Getting data before initialization"));
	}
	return Current == 0;
}

bool UCommandHistory::IsEmpty() const
{
	if (!IsInited)
	{
		UE_LOG(LogicalError, Error, TEXT("Getting data before initialization"));
	}
	return Commands.Num() == 0;
}

Command::Ptr UCommandHistory::GetNextCommand()
{
	if (!IsInited)
	{
		UE_LOG(LogicalError, Error, TEXT("Getting data before initialization"));
	}
	return (IsOnHead() || IsEmpty()) ? Command::Ptr(nullptr) : Commands[Current];
}

Command::Ptr UCommandHistory::GetPrevCommand()
{
	if (!IsInited)
	{
		UE_LOG(LogicalError, Error, TEXT("Getting data before initialization"));
	}
	return (IsOnTail() || IsEmpty()) ? Command::Ptr(nullptr) : Commands[Current-1];
}

void UCommandHistory::MoveForward()
{
	if (!IsInited)
	{
		UE_LOG(LogicalError, Error, TEXT("Accessing data before initialization"));
	}

	if (!IsOnHead())
	{
		++Current;
	}
	else
	{
		UE_LOG(LogicalError, Error, TEXT("Move forvard from the head of the history"));
	}
}

void UCommandHistory::MoveBackward()
{
	if (!IsInited)
	{
		UE_LOG(LogicalError, Error, TEXT("Accessing data before initialization"));
	}

	if (!IsOnTail()) {
		--Current;
	}
	else {
		UE_LOG(LogicalError, Error, TEXT("Move backward from the tail of the history"));
	}
}

void UCommandHistory::AddCommand(Command::Ptr command)
{
	if (!IsInited)
	{
		UE_LOG(LogicalError, Error, TEXT("Accessing data before initialization"));
	}

	// if we have some commands after the current then remove them
	ClearNextCommands();

	Commands.Push(command);
	++Current;
}

void UCommandHistory::ClearNextCommands()
{
	if (!IsInited)
	{
		UE_LOG(LogicalError, Error, TEXT("Accessing data before initialization"));
	}

	Commands.SetNum(Current);
}

TArray<FString> UCommandHistory::GetSerializedCommands() const
{
	if (!IsInited)
	{
		UE_LOG(LogicalError, Error, TEXT("Getting data before initialization"));
	}

	TArray<FString> Result;
	Result.Reserve(Commands.Num());

	for (const auto& Command : Commands)
	{
		Result.Push(Command->Serialize());
	}

	return Result;
}

void UCommandHistory::DeserializeCommands(const TArray<FString>& SerializedCommands)
{
	if (IsInited)
	{
		UE_LOG(LogicalError, Error, TEXT("Double initialization"));
		return;
	}

	Commands.Reserve(SerializedCommands.Num());

	for (const auto& Seralized : SerializedCommands)
	{
		Command::Ptr Command = CommandFactory::Get().CreateFromSerialized(Seralized);
		if (Command.IsValid())
		{
			Commands.Push(Command);
			Command->SetTarget(Reciever);
		}
		else
		{
			UE_LOG(LogicalError, Error, TEXT("Problem with deserializing commands"));
			Commands.Empty();
			Current = 0;
			return;
		}
	}
}

int UCommandHistory::GetCurrentCommand() const
{
	if (!IsInited)
	{
		UE_LOG(LogicalError, Error, TEXT("Getting data before initialization"));
	}

	return Current;
}

void UCommandHistory::SetCurrentCommand(int CommandIndex)
{
	if (IsInited)
	{
		UE_LOG(LogicalError, Error, TEXT("Double initialization"));
	}

	Current = CommandIndex;
}

int UCommandHistory::GetFirstChangableCommand() const
{
	if (!IsInited)
	{
		UE_LOG(LogicalError, Error, TEXT("Getting data before initialization"));
	}

	return FirstChangableCommand;
}

void UCommandHistory::SetFirstChangableCommand(int CommandIndex)
{
	if (IsInited)
	{
		UE_LOG(LogicalError, Error, TEXT("Double initialization"));
	}

	FirstChangableCommand = CommandIndex;
}

void UCommandHistory::SetInited()
{
	if (IsInited)
	{
		UE_LOG(LogicalError, Error, TEXT("Getting data before initialization"));
	}

	IsInited = true;
}

