#pragma once

namespace RC
{
	struct RotationAxis {
		enum Value {
			FX, RX,
			FY, RY,
			FZ, RZ,
			uninitialized
		} value;

		RotationAxis()
			: value(uninitialized)
		{}

		RotationAxis(Value _value)
			: value(_value)
		{}

		explicit RotationAxis(const FString& serialized)
			: value(uninitialized)
		{
			if (serialized == "FX") {
				value = FX;
			}
			else if (serialized == "FY") {
				value = FY;
			}
			else if (serialized == "FZ") {
				value = FZ;
			}
			else if (serialized == "RX") {
				value = RX;
			}
			else if (serialized == "RY") {
				value = RY;
			}
			else if (serialized == "RZ") {
				value = RZ;
			}
		}

		inline bool IsFrontal() const
		{
			return value == FX || value == FY || value == FZ;
		}

		inline bool IsX() const
		{
			return value == FX || value == RX;
		}

		inline bool IsY() const
		{
			return value == FY || value == RY;
		}

		inline bool IsZ() const
		{
			return value == FZ || value == RZ;
		}

		friend inline RotationAxis operator -(const RotationAxis& axis)
		{
			switch (axis.value)
			{
			case FX: return RX;
			case FY: return RY;
			case FZ: return RZ;
			case RX: return FX;
			case RY: return FY;
			case RZ: return FZ;
			default: return uninitialized;
			}
		}

		inline FString ToString() const
		{
			switch (value)
			{
			case FX: return "FX";
			case FY: return "FY";
			case FZ: return "FZ";
			case RX: return "RX";
			case RY: return "RY";
			case RZ: return "RZ";
			default: return "uninitialized";
			}
		}
	};
} // namespace RC
