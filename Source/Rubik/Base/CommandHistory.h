#pragma once

#include "Command.h"
#include "CommandHistory.generated.h"


UCLASS()
class UCommandHistory : public UObject
{
	GENERATED_BODY()

public:
	UCommandHistory();
	
	/** Set the actor that will be changed using commands */
	void SetReceiver(AActor* reciever);

	/** return true if there is not command in front of the current state */
	bool IsOnHead() const;
	/** retrun true if there is no command in back of the current state */
	bool IsOnTail(bool IgnoreChangeLimit = false) const;
	/** retrun true if history is empty */
	bool IsEmpty() const;
	/** return the next command or nullptr if history is empty */
	Command::Ptr GetNextCommand();
	/** return the previous command or nullptr if history is empty */
	Command::Ptr GetPrevCommand();
	/** Swith current history state to the next command */
	void MoveForward();
	/** Swith current history state to the previous command */
	void MoveBackward();
	/** Add new command in front of the current state (and remove all commands that were after the current state) */
	void AddCommand(Command::Ptr command);
	/** Remove all commands in front of the current */
	void ClearNextCommands();

	/** Get all the commands serialized as strings */
	UFUNCTION(BlueprintCallable)
	TArray<FString> GetSerializedCommands() const;
	/** Replace current commands with SerializedCommands (should be called before SetInited) */
	UFUNCTION(BlueprintCallable)
	void DeserializeCommands(const TArray<FString>& SerializedCommands);

	/** Get current command index */
	UFUNCTION(BlueprintCallable)
	int GetCurrentCommand() const;
	/** Set current command index (should be called before SetInited) */
	UFUNCTION(BlueprintCallable)
	void SetCurrentCommand(int CommandIndex);

	/** Get index of the first command that can be undone */
	UFUNCTION(BlueprintCallable)
	int GetFirstChangableCommand() const;
	/** Set index of the first command that can be undone (should be called before SetInited) */
	UFUNCTION(BlueprintCallable)
	void SetFirstChangableCommand(int CommandIndex);

	/** Mark history initialized (so we can't init data after this) */
	UFUNCTION(BlueprintCallable)
	void MarkInited();

	/** Undo all the executed commands */
	UFUNCTION(BlueprintCallable)
	void UnexecuteAll();

private:
	AActor* Reciever;
	TArray<Command::Ptr> Commands;
	int Current;
	int FirstChangableCommand;
	bool IsInited;
};
