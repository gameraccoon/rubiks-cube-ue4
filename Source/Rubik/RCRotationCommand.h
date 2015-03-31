#pragma once

#include "RCCommand.h"

namespace RC
{
	enum class RotationAxis {
		FX, RX,
		FY, RY,
		FZ, RZ
	};

	/// Rotation one of layers of a Rubik's cube
	class RotationCommand
		: public CubeCommand
	{
	public:
		static Ref Create(ARubicsCube * target, RotationAxis axis, int layerIdx);

		virtual ~RotationCommand();

		virtual void Execute() override;

		virtual void Unexecute() override;

		virtual bool IsContinious() override;

		virtual void SetProgress(float progress) override;

	private:
		RotationCommand(ARubicsCube * target, RotationAxis axis, int layerIdx);

	private:
		/// have command been already executed
		bool isExecuted;
		/// axis we rotating around (directed)
		const RotationAxis axis;
		/// index of layer that we rotating
		const int layerIndex;
	};
} // namespace RC
