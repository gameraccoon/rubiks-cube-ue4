#include "Rubik.h"
#include "RCRotationCommand.h"

#include "RubicsCube.h"

namespace RC
{
	void CubeCommand::SetTarget(AActor * receiver)
	{
		target = Cast<ARubicsCube>(receiver);

		if (!target)
		{
			UE_LOG(LogicalError, Error, TEXT("Wrong receiver for a command"));
		}
	}
	
	ARubicsCube * CubeCommand::GetTarget()
	{
		return target;
	}
}
