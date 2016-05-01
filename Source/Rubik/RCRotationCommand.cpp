#include "Rubik.h"
#include "RCRotationCommand.h"

#include "Base/CommandFactory.h"
#include "RubicsCube.h"

namespace RC
{
	static const FString ROTATION_COMMAND_ID = "RotationCommand";

	RotationCommand::RotationCommand(RotationAxis axis, int layerIdx)
		: CubeCommand()
		, isExecuted(false)
		, axis(axis)
		, layerIndex(layerIdx)
	{}

	RotationCommand::RotationCommand()
		: CubeCommand()
		, isExecuted(false)
		, layerIndex(0)
	{}

	RotationCommand::~RotationCommand()
	{
	}

	GameBase::Command::Ptr RotationCommand::Create(RotationAxis axis, int layerIdx)
	{
		return MakeShareable(new RotationCommand(axis, layerIdx));
	}

	GameBase::Command::Ptr RotationCommand::Create(TSharedPtr<FJsonObject> serialized)
	{
		Command::Ptr result = MakeShareable(new RotationCommand());
		result->InitFromJson(serialized);
		return result;
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

		ARubicsCube* cube = GetTarget();
		cube->Parts->RenewPartsLocations(axis, layerIndex);
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

		ARubicsCube* cube = GetTarget();
		cube->Parts->RenewPartsLocations(-axis, cube->GridSize - 1 - layerIndex);
	}

	bool RotationCommand::IsContinious()
	{
		return true;
	}

	void RotationCommand::SetProgress(float progress)
	{
		if (progress < -0.001f || progress > 1.001f)
		{
			FError::Throwf(TEXT("Incorrect progress value"));
			return;
		}

		ARubicsCube* cube = GetTarget();
		cube->Parts->RotateSlice(axis, layerIndex, progress * 90.0f);
	}

	TSharedPtr<FJsonObject> RotationCommand::ToJson() const
	{
		TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());
		result->SetStringField("axis", axis.ToString());
		result->SetNumberField("layerIndex", layerIndex);
		result->SetStringField("commandId", ROTATION_COMMAND_ID);
		return result;
	}

	void RotationCommand::InitFromJson(const TSharedPtr<FJsonObject> serialized)
	{
		axis = RC::RotationAxis(serialized->GetStringField("axis"));
		layerIndex = (int32)serialized->GetNumberField("layerIndex");
		isExecuted = false;
	}

	void RotationCommand::RegisterInFabric()
	{
		GameBase::CommandFactory::Get().RegisterCommand(ROTATION_COMMAND_ID, RC::RotationCommand::Create);
	}
}
