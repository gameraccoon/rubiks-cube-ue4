#include "Rubik.h"
#include "RCRotationCommand.h"

namespace RC
{
	RotationCommand::RotationCommand(ARubicsCube * target, RotationAxis axis, int layerIdx)
		: CubeCommand(target)
		, isExecuted(false)
		, axis(axis)
		, layerIndex(layerIdx)
	{}

	RotationCommand::~RotationCommand()
	{
	}

	GameBase::Command::Ref RotationCommand::Create(ARubicsCube * target, RotationAxis axis, int layerIdx)
	{
		return Ref(new RotationCommand(target, axis, layerIdx));
	}

	void RotationCommand::Execute()
	{
		if (isExecuted)
		{
			FError::Throwf(TEXT("Try to execute already executed command"));
			return;
		}

		isExecuted = true;
	}

	void RotationCommand::Unexecute()
	{
		if (!isExecuted)
		{
			FError::Throwf(TEXT("Try to unexecute not executed command"));
			return;
		}

		isExecuted = false;
	}

	bool RotationCommand::IsContinious()
	{
		return true;
	}

	void RotationCommand::SetProgress(float progress)
	{
		// ToDo: implementation
	}
}
