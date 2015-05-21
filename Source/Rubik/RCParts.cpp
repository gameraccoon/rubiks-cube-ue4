#include "Rubik.h"
#include "RCParts.h"

namespace RC
{
	CubeParts::CubeParts(int length, int width, int height, float _initialBlockSize, FVector _centerShift)
		: parts(length, width, height, PartInfo())
		, initialBlockSize(_initialBlockSize)
		, centerShift(_centerShift)
		, mainLocation(0.0f, 0.0f, 0.0f)
		, mainRotation(0.0f, 0.0f, 0.0f)
	{
	}

	void CubeParts::InsertPart(AActor* part, Coord pos)
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

		FVector location(FVector(pos.x, pos.y, pos.z) * initialBlockSize + centerShift);
		FRotator roataion(GetPartInitialRotation(pos));

		part->SetActorLocation(mainLocation + mainRotation.RotateVector(location));
		part->SetActorRotation(mainRotation + roataion);

		partInfo.ptr = part;
		partInfo.initialLocation = location;
		partInfo.initialRotation = FQuat(roataion);
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
		for (unsigned int y = 0, ySize = slice.getWidth(); y < ySize; ++y)
		{
			for (unsigned int x = 0, xSize = slice.getLength(); x < xSize; ++x)
			{
				RotatePart(slice[x][y], rotation, center);
			}
		}
	}


	void CubeParts::RenewPartsLocations(RotationAxis axis, int pos)
	{
		Slice slice = GetSlice(axis, pos);

		const int N = slice.getWidth();

		if (!axis.IsFrontal())
		{
			for (int ii = 0; ii < (N / 2 + N % 2); ++ii) {
				for (int jj = 0; jj < N / 2; ++jj) {
					auto tmp = *slice[ii][jj];
					*slice[ii][jj] = *slice[N - 1 - jj][ii];
					*slice[N - 1 - jj][ii] = *slice[N - 1 - ii][N - 1 - jj];
					*slice[N - 1 - ii][N - 1 - jj] = *slice[jj][N - 1 - ii];
					*slice[jj][N - 1 - ii] = tmp;
				}
			}
		}
		else
		{
			for (int ii = 0; ii < (N / 2 + N % 2); ++ii) {
				for (int jj = 0; jj < N / 2; ++jj) {
					auto tmp = *slice[ii][jj];
					*slice[ii][jj] = *slice[jj][N - 1 - ii];
					*slice[jj][N - 1 - ii] = *slice[N - 1 - ii][N - 1 - jj];
					*slice[N - 1 - ii][N - 1 - jj] = *slice[N - 1 - jj][ii];
					*slice[N - 1 - jj][ii] = tmp;
				}
			}
		}

		for (unsigned int z = 0, zSize = parts.getHeight(); z < zSize; ++z)
		{
			for (unsigned int y = 0, ySize = parts.getWidth(); y < ySize; ++y)
			{
				for (unsigned int x = 0, xSize = parts.getLength(); x < xSize; ++x)
				{
					if (!parts[x][y][z].ptr)
					{
						continue;
					}

					FVector location(FVector(x, y, z) * initialBlockSize + centerShift);
					FRotator roataion(GetPartInitialRotation(Coord(x, y, z)));

					PartInfo &partInfo = parts[x][y][z];
					partInfo.ptr->SetActorLocation(mainLocation + mainRotation.RotateVector(location));
					//partInfo.ptr->SetActorRotation(mainRotation + roataion);

					auto* partPtr = partInfo.ptr;
					partInfo.initialRotation = FQuat(roataion);
					partInfo.initialLocation = location;
				}
			}
		}
	}

	void CubeParts::RotatePart(PartInfo* part, const FRotator& rotation, const FVector& center)
	{
		if (!part->ptr)
		{
			return;
		}

		FQuat rotationQuat(rotation);
		part->ptr->SetActorLocation(mainLocation + mainRotation.RotateVector(rotationQuat.RotateVector(part->initialLocation)));
		part->ptr->SetActorRotation(mainRotation + (rotationQuat * part->initialRotation).Rotator());
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
					slice[x][z] = &parts[x][y][z];
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

	// ToDo: find any better way to calculate rotation
	FRotator CubeParts::GetPartInitialRotation(const Coord& coord)
	{
		if (coord.x == 0)
		{
			if (coord.y == 0)
			{
				if (coord.z == 0)
				{
					return FRotator(0.0f, -90.0f, -90.0f);
				}
				else if (coord.z == parts.getHeight() - 1)
				{
					return FRotator(90.0f, -90.0f, -90.0f);
				}

				return FRotator(90.0f, 90.0f, 90.0f);
			}
			else if (coord.y == parts.getWidth() - 1)
			{
				if (coord.z == 0)
				{
					return FRotator(-90.0f, -90.0f, -90.0f);
				}
				else if (coord.z == parts.getHeight() - 1)
				{
					return FRotator(0.0f, 90.0f, 90.0f);
				}

				return FRotator(-90.0f, 180.0f, 0.0f);
			}

			if (coord.z == 0)
			{
				return FRotator(0.0f, -90.0f, -90.0f);
			}
			else if (coord.z == parts.getHeight() - 1)
			{
				return FRotator(0.0f, 90.0f, 90.0f);
			}

			return FRotator(90.0f, 0.0f, 0.0f);
		}
		else if (coord.x == parts.getLength() - 1)
		{
			if (coord.y == 0)
			{
				if (coord.z == 0)
				{
					return FRotator(0.0f, 0.0f, -90.0f);
				}
				else if (coord.z == parts.getHeight() - 1)
				{
					return FRotator(0.0f, 0.0f, 0.0f);
				}

				return FRotator(-90.0f, 0.0f, 0.0f);
			}
			else if (coord.y == parts.getWidth() - 1)
			{
				if (coord.z == 0)
				{
					return FRotator(0.0f, 0.0f, 180.0f);
				}
				else if (coord.z == parts.getHeight() - 1)
				{
					return FRotator(0.0f, 90.0f, 0.0f);
				}

				return FRotator(-90.0f, 90.0f, 0.0f);
			}

			if (coord.z == 0)
			{
				return FRotator(0.0f, 90.0f, -90.0f);
			}
			else if (coord.z == parts.getHeight() - 1)
			{
				return FRotator(0.0f, 90.0f, 0.0f);
			}

			return FRotator(-90.0f, 0.0f, 0.0f);
		}

		if (coord.y == 0)
		{
			if (coord.z == 0)
			{
				return FRotator(0.0f, 0.0f, -90.0f);
			}
			else if (coord.z == parts.getHeight() - 1)
			{
				return FRotator(0.0f, 0.0f, 0.0f);
			}

			return FRotator(0.0f, 0.0f, -90.0f);
		}
		else if (coord.y == parts.getWidth() - 1)
		{
			if (coord.z == 0)
			{
				return FRotator(0.0f, 0.0f, 180.0f);
			}
			else if (coord.z == parts.getHeight() - 1)
			{
				return FRotator(0.0f, 0.0f, 90.0f);
			}

			return FRotator(0.0f, 0.0f, 90.0f);
		}

		if (coord.z == 0)
		{
			return FRotator(180.0f, 0.0f, 0.0f);
		}
		else if (coord.z == parts.getHeight() - 1)
		{
			return FRotator(0.0f, 0.0f, 0.0f);
		}

		FError::Throwf(TEXT("Wrong actor position to rotate"));
		return FRotator::ZeroRotator;
	}


}; // namespace RC
