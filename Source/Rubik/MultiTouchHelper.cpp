#include "Rubik.h"
#include "MultitouchHelper.h"

namespace Utils
{
	MultitouchHelper::MultitouchHelper()
	{
		TouchesCenter.RefUpdateDelegate().BindRaw(this, &MultitouchHelper::CalcTouchesCenterCallback);
		SpinAngle.RefUpdateDelegate().BindRaw(this, &MultitouchHelper::CalcSpinAngleCallback);
		PitchSize.RefUpdateDelegate().BindRaw(this, &MultitouchHelper::CalcPitchSizeCallback);
		MoveVector.RefUpdateDelegate().BindRaw(this, &MultitouchHelper::CalcMoveVectorCallback);
	}

	void MultitouchHelper::AddTouch(uint32 Handle, FVector2D TouchLocation)
	{
		Touches.Add(Handle, TouchInfo(TouchLocation));
		ResetAllParameters();
	}

	void MultitouchHelper::RemoveTouch(uint32 Handle)
	{
		Touches.Remove(Handle);
		ResetAllParameters();
	}

	void MultitouchHelper::MoveTouch(uint32 Handle, FVector2D TouchLocation)
	{
		TouchInfo* touchInfo = Touches.Find(Handle);
		if (touchInfo)
		{
			touchInfo->LastFrameDelta = TouchLocation - touchInfo->TouchLocation;
			touchInfo->TouchLocation = TouchLocation;
		}

		ResetAllParameters();
	}

	int MultitouchHelper::GetTouchesCount() const
	{
		return Touches.Num();
	}

	FVector2D MultitouchHelper::CalcTouchesCenterCallback()
	{
		FVector2D TouchesSum(0.0f, 0.0f);
		for (auto TouchesIt = Touches.CreateConstIterator(); TouchesIt; ++TouchesIt)
		{
			TouchesSum += TouchesIt->Value.TouchLocation;
		}

		int touchesNum = Touches.Num();
		if (touchesNum > 0)
		{
			return TouchesSum / touchesNum;
		}
		else
		{
			return FVector2D(0.0f, 0.0f);
		}
	}

	float MultitouchHelper::CalcSpinAngleCallback()
	{
		int touchesNum = Touches.Num();

		if (touchesNum <= 1)
		{
			return 0.0f;
		}

		FVector2D center = TouchesCenter.Get();

		float spinSum = 0.0f;
		for (auto TouchesIt = Touches.CreateConstIterator(); TouchesIt; ++TouchesIt)
		{
			if (TouchesIt->Value.LastFrameDelta.SizeSquared() > 0.0f)
			{
				FVector2D oldDir = (TouchesIt->Value.TouchLocation - TouchesIt->Value.LastFrameDelta) - center;
				FVector2D newDir = TouchesIt->Value.TouchLocation - center;
				float angle = FMath::Atan2(newDir.X, newDir.Y) - FMath::Atan2(oldDir.X, oldDir.Y);
				if (angle > PI) { angle -= 2 * PI; }
				if (angle < -PI) { angle += 2 * PI; }
				spinSum += angle;
			}
		}

		return 3 * FMath::RadiansToDegrees(spinSum) / touchesNum;
	}

	float MultitouchHelper::CalcPitchSizeCallback()
	{
		int touchesNum = Touches.Num();

		if (touchesNum <= 1)
		{
			return 0.0f;
		}

		FVector2D center = TouchesCenter.Get();

		float pitchSum = 0.0f;
		for (auto TouchesIt = Touches.CreateConstIterator(); TouchesIt; ++TouchesIt)
		{
			FVector2D fromCenter = center - (TouchesIt->Value.TouchLocation - TouchesIt->Value.LastFrameDelta);
			float fromCenterSize = fromCenter.Size();
			if (fromCenterSize != 0.0f && TouchesIt->Value.LastFrameDelta.SizeSquared() > 0.0f)
			{
				pitchSum += FVector2D::DotProduct(fromCenter, TouchesIt->Value.LastFrameDelta) / fromCenterSize;
			}
		}

		return pitchSum / touchesNum;
	}

	FVector2D MultitouchHelper::CalcMoveVectorCallback()
	{
		int touchesNum = Touches.Num();

		if (touchesNum == 0)
		{
			return FVector2D(0.0f, 0.0f);
		}

		FVector2D MoveSum(0.0f, 0.0f);
		for (auto TouchesIt = Touches.CreateConstIterator(); TouchesIt; ++TouchesIt)
		{
			MoveSum += TouchesIt->Value.LastFrameDelta;
		}

		return MoveSum / touchesNum;
	}

	void MultitouchHelper::ResetAllParameters()
	{
		TouchesCenter.Unactualize();
		SpinAngle.Unactualize();
		PitchSize.Unactualize();
		MoveVector.Unactualize();
	}

}; // namespace RC
