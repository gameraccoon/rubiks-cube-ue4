#pragma once

#include "RCCommand.h"
#include "RCRotationAxis.h"

namespace RC
{
	/// Rotation one of layers of a Rubik's cube
	class RotationCommand
		: public CubeCommand
	{
	public:
		static Ptr Create(RotationAxis axis, int layerIdx);
		static Ptr Create(TSharedPtr<FJsonObject> serialized);

		virtual ~RotationCommand();

		virtual void Execute() override;
		virtual void Unexecute() override;

		virtual bool IsContinious() override;
		virtual void SetProgress(float progress) override;

		virtual TSharedPtr<FJsonObject> ToJson() const override;
		virtual void InitFromJson(const TSharedPtr<FJsonObject> serialized) override;

		static void RegisterInFabric();

	private:
		RotationCommand();
		RotationCommand(RotationAxis axis, int layerIdx);

	private:
		/// have command been already executed
		bool isExecuted;
		/// axis we rotating around (directed)
		RotationAxis axis;
		/// index of layer that we rotating
		int32 layerIndex;
	};
} // namespace RC
