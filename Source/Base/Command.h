#pragma once

#include "Engine.h"

namespace GameBase
{
	// An abstract action that we can do and undo
	class Command
	{
	public:
		typedef TSharedRef<Command> Ref;
		typedef TSharedPtr<Command> Ptr;

	public:
		virtual ~Command() = default;

		/// execute the command
		virtual void Execute() = 0;
		/// undo actions of the command
		virtual void Unexecute() = 0;
		/// is command continious in time
		virtual bool IsContinious() = 0;
		/// set progress (for continious commands)
		virtual void SetProgress(float progress) = 0;
	};
} // namespace GameBase
