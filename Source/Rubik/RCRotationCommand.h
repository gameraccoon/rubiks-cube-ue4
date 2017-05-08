#pragma once

#include "RCCommand.h"
#include "RCRotationAxis.h"

namespace RC
{
	/** Rotation one of layers of a Rubik's cube */
	class RotationCommand
		: public CubeCommand
	{
	public:
		static Ptr Create(RotationAxis Axis, int LayerIdx);
		static Ptr Create(const TSharedPtr<FJsonObject> Serialized);

		virtual ~RotationCommand();

		virtual void Execute() override;
		virtual void Unexecute() override;

		virtual bool IsContinious() override;
		virtual void SetProgress(float Progress) override;

		virtual FString Serialize() const override;
		virtual void Deserialize(const TSharedPtr<FJsonObject> Serialized) override;

		virtual FString GetType() const override;

		static void RegisterInFabric();

	private:
		RotationCommand();
		RotationCommand(RotationAxis Axis, int LayerIdx);

	private:
		/** have command been already executed */
		bool IsExecuted;
		/** axis we rotating around (directed) */
		RotationAxis Axis;
		/** index of layer that we rotating */
		int32 LayerIndex;
	};
} // namespace RC
