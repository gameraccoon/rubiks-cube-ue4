#pragma once

#include "Base/Command.h"
#include "RubicsCube.h"

namespace RC
{
	/// some abstract action with Rubik's cube
	class CubeCommand
		: public GameBase::Command
	{
	public:
		CubeCommand(ARubicsCube * target)
			: target(target)
		{}

	protected:
		/// get target for modification
		ARubicsCube * GetTarget() { return target; }

	private:
		/// pointer to object that we will modify
		ARubicsCube * const target;
	};
} // namespace RC
