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
			Coord(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
			int x, y, z;
		};

	public:
		CubeParts(int length, int width, int height, float initialBlockSize, FVector centerShift);

		void InsertPart(AActor* part, Coord pos);
		void RotateSlice(RotationAxis axis, int pos, float angle);
		void RenewPartsLocations(RotationAxis axis, int pos);

		inline void SetMainLocation(const FVector& location) { mainLocation = location; };
		inline void SetMainRotation(const FQuat& rotation) { mainRotation = rotation; };

		void UpdateAllParts();

	private:
		typedef AActor* PartPtr;

		struct PartInfo {
			PartPtr ptr;
			FQuat baseRotation;
			FVector baseLocation;

			FQuat localRotation;
			FVector localLocation;

			PartInfo() : ptr(nullptr) {}
		};

		typedef Matrix<PartInfo*> Slice;

	private:
		void RotatePart(PartInfo* part, const FRotator& rotation);
		Slice GetSlice(RotationAxis axis, int pos);
		FRotator GetPartInitialRotation(const Coord& coord);
		void RotateSlice(Slice& slice, RotationAxis axis, float angle);
		void MakeSnapshot(Slice& slice);

		void RearrangeControls();
		void RearrangePartControls(AActor * side, const Coord& coord);
		void RearrangeSideControls(AActor * side, const Coord& coord);

	private:
		Array3D<PartInfo> parts;
		float initialBlockSize;
		FVector centerShift;

		FVector mainLocation;
		FQuat mainRotation;
	};
} // namespace RC
