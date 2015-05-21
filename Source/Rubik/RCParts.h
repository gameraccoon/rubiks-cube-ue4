#pragma once

#include "Base/Matrix.h"
#include "RCRotationAxis.h"
#include "Engine.h"

namespace RC
{
	class CubeParts
	{
	public:
		struct Coord {
			Coord(int x, int y, int z) : x(x), y(y), z(z) {}
			int x, y, z;
		};

	public:
		CubeParts(int length, int width, int height, float initialBlockSize, FVector centerShift);

		void InsertPart(AActor* part, Coord pos);
		void RotateSlice(RotationAxis axis, int pos, float angle, const FVector& center);
		void RenewPartsLocations(RotationAxis axis, int pos);

		inline void SetMainLocation(const FVector& location) { mainLocation = location; };
		inline void SetMainRotation(const FRotator& rotation) { mainRotation = rotation; };

	private:
		typedef AActor* PartPtr;

		struct PartInfo {
			PartPtr ptr;
			FQuat initialRotation;
			FVector initialLocation;

			PartInfo() : ptr(nullptr) {}
		};

		typedef GameBase::Matrix<PartInfo*> Slice;

	private:
		void RotatePart(PartInfo* part, const FRotator& rotation, const FVector& center);
		Slice GetSlice(RotationAxis axis, int pos);
		FRotator GetPartInitialRotation(const Coord& coord);

	private:
		GameBase::Array3D<PartInfo> parts;
		float initialBlockSize;
		FVector centerShift;

		FVector mainLocation;
		FRotator mainRotation;
	};
} // namespace GameBase
