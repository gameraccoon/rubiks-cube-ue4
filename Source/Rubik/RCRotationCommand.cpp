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
		SetProgress(1.0f);
	}

	void RotationCommand::Unexecute()
	{
		if (!isExecuted)
		{
			FError::Throwf(TEXT("Try to unexecute not executed command"));
			return;
		}

		isExecuted = false;
		SetProgress(0.0f);
	}

	bool RotationCommand::IsContinious()
	{
		return true;
	}

	void RotationCommand::SetProgress(float progress)
	{
		if (progress < 0.001f || progress > 0.999f)
		{
			FError::Throwf(TEXT("Incorrect progress value"));
			return;
		}

		ARubicsCube* cube = GetTarget();
		cube->parts.RotateSlice(axis, layerIndex, progress * 90.0f, FVector());
	}
}
