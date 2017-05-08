#pragma once

#include "Base/Command.h"

// fwd
class ARubicsCube;

namespace RC
{

	/// some abstract action with Rubik's cube
	class CubeCommand : public Command
	{
	public:
		virtual void SetTarget(AActor* receiver);

	protected:
		/// get target for modification
		ARubicsCube * GetTarget();

	private:
		/// pointer to object that we will modify
		ARubicsCube * target;
	};
} // namespace RC
