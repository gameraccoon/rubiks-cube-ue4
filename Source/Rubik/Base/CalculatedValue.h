#pragma once

#include "Engine.h"

template<typename T>
class CalculatedValue
{
public:
	DECLARE_DELEGATE_RetVal(T, Delegate);

	CalculatedValue()
		: IsActual(false)
		, Value()
	{}

	T Get()
	{
		if (!IsActual)
		{
			Value = UpdateDelegate.Execute();
			IsActual = true;
		}

		return Value;
	}

	inline void Unactualize()
	{
		IsActual = false;
	}

	inline Delegate& RefUpdateDelegate()
	{
		return UpdateDelegate;
	}

private:
	T Value;
	bool IsActual;
	Delegate UpdateDelegate;
};
