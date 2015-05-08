#include "Rubik.h"
#include "RCParts.h"

namespace RC
{
	CubeParts::CubeParts(int length, int width, int height)
		: parts(length, width, height, PartInfo())
	{
	}

	void CubeParts::InsertPart(UStaticMeshComponent* part, Coord pos)
	{
		PartInfo& partInfo = parts[pos.x][pos.y][pos.z];

		if (partInfo.ptr)
		{
			FError::Throwf(TEXT("Reassignation of cube part"));
		}

		if (!part)
		{
			FError::Throwf(TEXT("Part is NULL"));
		}

		partInfo.ptr = part;
		partInfo.initialRotation = FQuat(part->GetComponentRotation());
		partInfo.initialLocation = part->GetComponentLocation();
	}

	void CubeParts::RotateSlice(RotationAxis axis, int pos, float angle, const FVector& center)
	{
		const bool isXMove = axis.IsX();
		const bool isYMove = axis.IsY();
		const bool isZMove = axis.IsZ();

		const bool isReversed = !axis.IsFrontal();

		FRotator rotation;

		if (isXMove)
		{
			rotation = FRotator(0.0f, 0.0f, isReversed ? -angle : angle);
		}
		else if (isYMove)
		{
			rotation = FRotator(isReversed ? -angle : angle, 0.0f, 0.0f);
		}
		else if (isZMove)
		{
			rotation = FRotator(0.0f, isReversed ? -angle : angle, 0.0f);
		}

		Slice slice = GetSlice(axis, pos);
		for (unsigned int y = 0, ySize = slice.getHeight(); y < ySize; ++y)
		{
			for (unsigned int x = 0, xSize = slice.getWidth(); x < xSize; ++x)
			{
				RotatePart(slice[x][y], rotation, center);
			}
		}
	}


	void CubeParts::RenewPartsLocations(RotationAxis axis, int pos)
	{
		//const bool isReversed = !axis.IsFrontal();
		//Slice slice = GetSlice(axis, pos);

		//const int N = slice.getWidth();

		//if (!isReversed)
		//{
		//	for (int ii = 0; ii < (N / 2 + N % 2); ++ii) {
		//		for (int jj = 0; jj < N / 2; ++jj) {
		//			auto tmp = *slice[ii][jj];
		//			*slice[ii][jj] = *slice[N - 1 - jj][ii];
		//			*slice[N - 1 - jj][ii] = *slice[N - 1 - ii][N - 1 - jj];
		//			*slice[N - 1 - ii][N - 1 - jj] = *slice[jj][N - 1 - ii];
		//			*slice[jj][N - 1 - ii] = tmp;
		//		}
		//	}
		//}
		//else
		//{
		//	for (int ii = 0; ii < (N / 2 + N % 2); ++ii) {
		//		for (int jj = 0; jj < N / 2; ++jj) {
		//			auto tmp = *slice[ii][jj];
		//			*slice[ii][jj] = *slice[jj][N - 1 - ii];
		//			*slice[jj][N - 1 - ii] = *slice[N - 1 - ii][N - 1 - jj];
		//			*slice[N - 1 - ii][N - 1 - jj] = *slice[N - 1 - jj][ii];
		//			*slice[N - 1 - jj][ii] = tmp;
		//		}
		//	}
		//}

		//for (unsigned int y = 0, ySize = slice.getHeight(); y < ySize; ++y)
		//{
		//	for (unsigned int x = 0, xSize = slice.getWidth(); x < xSize; ++x)
		//	{
		//		if (slice[x][y]->ptr)
		//		{
		//			//slice[x][y]->initialLocation = slice[x][y]->ptr->GetComponentLocation();
		//			slice[x][y]->initialRotation = FQuat(slice[x][y]->ptr->GetComponentRotation());
		//		}
		//	}
		//}
	}

	void CubeParts::RotatePart(PartInfo* part, const FRotator& rotation, const FVector& center)
	{
		if (!part->ptr)
		{
			return;
		}

		FQuat rotationQuat(rotation);
		part->ptr->SetRelativeLocation(rotationQuat.RotateVector(part->initialLocation));
		part->ptr->SetRelativeRotation((rotationQuat * part->initialRotation).Rotator());
	}

	CubeParts::Slice CubeParts::GetSlice(RotationAxis axis, int pos)
	{
		Slice slice(parts.getLength(), parts.getLength(), nullptr);

		const bool isXMove = axis.IsX();
		const bool isYMove = axis.IsY();
		const bool isZMove = axis.IsZ();

		const bool isReversed = !axis.IsFrontal();

		const int slicePos = isReversed ? parts.getWidth() - pos - 1 : pos;

		if ((isXMove && isYMove) || (isYMove && isZMove) || (isXMove && isZMove))
		{
			FError::Throwf(TEXT("Calculation error"));
			return slice;
		}

		if (isXMove)
		{
			const int x = slicePos;

			for (unsigned int y = 0, ySize = parts.getWidth(); y < ySize; ++y)
			{
				for (unsigned int z = 0, zSize = parts.getHeight(); z < zSize; ++z)
				{
					slice[z][y] = &parts[x][y][z];
				}
			}
		}
		else if (isYMove)
		{
			const int y = slicePos;

			for (unsigned int x = 0, xSize = parts.getLength(); x < xSize; ++x)
			{
				for (unsigned int z = 0, zSize = parts.getHeight(); z < zSize; ++z)
				{
					slice[z][x] = &parts[x][y][z];
				}
			}
		}
		else if (isZMove)
		{
			const int z = slicePos;

			for (unsigned int y = 0, ySize = parts.getWidth(); y < ySize; ++y)
			{
				for (unsigned int x = 0, xSize = parts.getLength(); x < xSize; ++x)
				{
					slice[x][y] = &parts[x][y][z];
				}
			}
		}
		else
		{
			FError::Throwf(TEXT("Calculation error"));
		}

		return slice;
	}

}; // namespace RC
