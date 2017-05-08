#pragma once

#include "Engine.h"

/** An abstract action that we can do and undo */
class Command
{
public:
	typedef TSharedPtr<Command> Ptr;

public:
	virtual ~Command() = default;

	/** execute the command */
	virtual void Execute() = 0;
	/** undo actions of the command */
	virtual void Unexecute() = 0;
	/** is command continious in time */
	virtual bool IsContinious() = 0;
	/** set progress (for continious commands) */
	virtual void SetProgress(float progress) = 0;
	/** set the actor that will be affected by the command */
	virtual void SetTarget(AActor* target) = 0;

	virtual FString GetType() const = 0;

	virtual FString Serialize() const = 0;
	virtual void Deserialize(const TSharedPtr<FJsonObject> Serialized) = 0;
};
