#include "Rubik.h"
#include "RCRotationCommand.h"

#include "Base/CommandFactory.h"
#include "RubicsCube.h"

namespace RC
{
	static const FString ROTATION_COMMAND_ID = "RotationCommand";

	RotationCommand::RotationCommand(RotationAxis Axis, int LayerIdx)
		: CubeCommand()
		, IsExecuted(false)
		, Axis(Axis)
		, LayerIndex(LayerIdx)
	{}

	RotationCommand::RotationCommand()
		: CubeCommand()
		, IsExecuted(false)
		, LayerIndex(0)
	{}

	RotationCommand::~RotationCommand()
	{
	}

	Command::Ptr RotationCommand::Create(RotationAxis Axis, int LayerIdx)
	{
		return MakeShareable(new RotationCommand(Axis, LayerIdx));
	}

	Command::Ptr RotationCommand::Create(const TSharedPtr<FJsonObject> Serialized)
	{
		Command::Ptr result = MakeShareable(new RotationCommand());
		result->Deserialize(Serialized);
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

		ARubicsCube* Cube = GetTarget();
		Cube->Parts->RenewPartsLocations(-Axis, Cube->GridSize - 1 - LayerIndex);
	}

	bool RotationCommand::IsContinious()
	{
		return true;
	}

	void RotationCommand::SetProgress(float Progress)
	{
		if (Progress < -0.001f || Progress > 1.001f)
		{
			UE_LOG(LogicalError, Error, TEXT("Incorrect progress value"));
			return;
		}

		ARubicsCube* Cube = GetTarget();
		Cube->Parts->RotateSlice(Axis, LayerIndex, Progress * 90.0f);
	}

	FString RotationCommand::Serialize() const
	{
		TSharedRef<FJsonObject> Json = MakeShareable(new FJsonObject());
		Json->SetStringField("type", GetType());
		Json->SetStringField("axis", Axis.ToString());
		Json->SetNumberField("layerIndex", LayerIndex);
		Json->SetStringField("commandId", ROTATION_COMMAND_ID);
		Json->SetBoolField("isExecuted", IsExecuted);

		FString OutputString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(Json, Writer);
		return OutputString;
	}

	void RotationCommand::Deserialize(const TSharedPtr<FJsonObject> Serialized)
	{
		Axis = RC::RotationAxis(Serialized->GetStringField("axis"));
		LayerIndex = (int32)Serialized->GetNumberField("layerIndex");
		IsExecuted = Serialized->GetBoolField("isExecuted");
	}

	FString RotationCommand::GetType() const
	{
		return ROTATION_COMMAND_ID;
	}

	void RotationCommand::RegisterInFabric()
	{
		CommandFactory::Get().RegisterCommand(ROTATION_COMMAND_ID, RC::RotationCommand::Create);
	}
}
