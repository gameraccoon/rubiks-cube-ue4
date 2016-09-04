#include "Rubik.h"
#include "RCRotationCommand.h"

#include "Base/CommandFactory.h"
#include "RubicsCube.h"

namespace RC
{
	static const FString ROTATION_COMMAND_ID = "RotationCommand";

	RotationCommand::RotationCommand(RotationAxis axis, int layerIdx)
		: CubeCommand()
		, IsExecuted(false)
		, Axis(axis)
		, LayerIndex(layerIdx)
	{}

	RotationCommand::RotationCommand()
		: CubeCommand()
		, IsExecuted(false)
		, LayerIndex(0)
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
		if (IsExecuted)
		{
			UE_LOG(LogicalError, Error, TEXT("Try to execute already executed command"));
			return;
		}

		IsExecuted = true;
		SetProgress(1.0f);

		ARubicsCube* cube = GetTarget();
		cube->Parts->RenewPartsLocations(Axis, LayerIndex);
	}

	void RotationCommand::Unexecute()
	{
		if (!IsExecuted)
		{
			UE_LOG(LogicalError, Error, TEXT("Try to unexecute not executed command"));
			return;
		}

		IsExecuted = false;
		SetProgress(0.0f);

		ARubicsCube* cube = GetTarget();
		cube->Parts->RenewPartsLocations(-Axis, cube->GridSize - 1 - LayerIndex);
	}

	bool RotationCommand::IsContinious()
	{
		return true;
	}

	void RotationCommand::SetProgress(float progress)
	{
		if (progress < -0.001f || progress > 1.001f)
		{
			UE_LOG(LogicalError, Error, TEXT("Incorrect progress value"));
			return;
		}

		ARubicsCube* cube = GetTarget();
		cube->Parts->RotateSlice(Axis, LayerIndex, progress * 90.0f);
	}

	TSharedPtr<FJsonObject> RotationCommand::ToJson() const
	{
		TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());
		result->SetStringField("axis", Axis.ToString());
		result->SetNumberField("layerIndex", LayerIndex);
		result->SetStringField("commandId", ROTATION_COMMAND_ID);
		return result;
	}

	void RotationCommand::InitFromJson(const TSharedPtr<FJsonObject> serialized)
	{
		Axis = RC::RotationAxis(serialized->GetStringField("axis"));
		LayerIndex = (int32)serialized->GetNumberField("layerIndex");
		IsExecuted = false;
	}

	void RotationCommand::RegisterInFabric()
	{
		GameBase::CommandFactory::Get().RegisterCommand(ROTATION_COMMAND_ID, RC::RotationCommand::Create);
	}
}
