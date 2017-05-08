#pragma once

#include "Engine.h"
#include "Base/CalculatedValue.h"

namespace Utils
{
	class MultitouchHelper
	{
	public:
		MultitouchHelper();

		void AddTouch(uint32 Handle, FVector2D TouchLocation);
		void RemoveTouch(uint32 Handle);
		void MoveTouch(uint32 Handle, FVector2D TouchLocation);

		FVector2D GetTouchesCenter() { return TouchesCenter.Get(); }
		float GetSpinAngle() { return SpinAngle.Get(); }
		float GetPitchSize() { return PitchSize.Get(); }
		FVector2D GetMoveVector() { return MoveVector.Get(); }
		int GetTouchesCount() const;

	private:
		struct TouchInfo
		{
			TouchInfo(FVector2D touchLocation)
				: TouchLocation(touchLocation)
				, LastFrameDelta(FVector2D(0.0f, 0.0f))
			{}

			FVector2D TouchLocation;
			FVector2D LastFrameDelta;
		};

	private:
		FVector2D CalcTouchesCenterCallback();
		float CalcSpinAngleCallback();
		float CalcPitchSizeCallback();
		FVector2D CalcMoveVectorCallback();

		void ResetAllParameters();

	private:
		TMap<uint32, TouchInfo> Touches;

		CalculatedValue<FVector2D> TouchesCenter;
		CalculatedValue<float> SpinAngle;
		CalculatedValue<float> PitchSize;
		CalculatedValue<FVector2D> MoveVector;
	};
} // namespace GameBase
