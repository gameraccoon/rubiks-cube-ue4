#pragma once

#include "Base/Matrix.h"
#include "Engine.h"

namespace RC
{
	enum class RotationAxis {
		FX, RX,
		FY, RY,
		FZ, RZ
	};

	class CubeParts
	{
	public:
		struct Coord {
			Coord(int x, int y, int z) : x(x), y(y), z(z) {}
			int x, y, z;
		};

	public:
		CubeParts(int length, int width, int height);

		void InsertPart(UStaticMeshComponent* part, Coord pos);
		void RotateSlice(RotationAxis axis, int pos, float angle, const FVector& center);

	private:
		typedef UStaticMeshComponent* PartPtr;

	private:
		GameBase::Array3D<PartPtr> parts;
		void RotatePart(PartPtr part, const FRotator& rotation, const FVector& center);
	};
} // namespace GameBase
