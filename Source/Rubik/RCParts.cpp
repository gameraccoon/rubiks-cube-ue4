#include "Rubik.h"
#include "RCParts.h"

namespace RC
{
	CubeParts::CubeParts(int length, int width, int height)
		: parts(length, width, height, nullptr)
	{
	}

	void CubeParts::InsertPart(UStaticMeshComponent* part, Coord pos)
	{
		PartPtr& partPtr = parts[pos.x][pos.y][pos.z];

		if (partPtr)
		{
			FError::Throwf(TEXT("Reassignation of cube part"));
		}

		partPtr = part;
	}

	void CubeParts::RotateSlice(RotationAxis axis, int pos, float angle, const FVector& center)
	{
		const bool isXMove = axis == RotationAxis::FX || axis == RotationAxis::RX;
		const bool isYMove = axis == RotationAxis::FY || axis == RotationAxis::RY;
		const bool isZMove = axis == RotationAxis::FZ || axis == RotationAxis::RZ;

		const bool isReversed = axis == RotationAxis::RX || axis == RotationAxis::RY || axis == RotationAxis::RZ;

		const int slicePos = isReversed ? parts.getWidth() - pos - 1 : pos;

		if ((isXMove && isYMove) || (isYMove && isZMove) || (isXMove && isZMove))
		{
			FError::Throwf(TEXT("Calculation error"));
			return;
		}

		if (isXMove)
		{
			FRotator rotation(isReversed ? -angle : angle, 0.0f, 0.0f);

			const int x = slicePos;

			for (int y = 0, ySize = parts.getWidth(); y < ySize; ++y)
			{
				for (int z = 0, zSize = parts.getHeight(); z < zSize; ++z)
				{
					RotatePart(parts[x][y][z], rotation, center);
				}
			}
		}
		else if (isYMove)
		{
			FRotator rotation(0.0f, isReversed ? -angle : angle, 0.0f);

			const int y = slicePos;

			for (int x = 0, xSize = parts.getLength(); x < xSize; ++x)
			{
				for (int z = 0, zSize = parts.getHeight(); z < zSize; ++z)
				{
					RotatePart(parts[x][y][z], rotation, center);
				}
			}
		}
		else if (isZMove)
		{
			FRotator rotation(0.0f, 0.0f, isReversed ? -angle : angle);

			const int z = slicePos;

			for (int y = 0, ySize = parts.getWidth(); y < ySize; ++y)
			{
				for (int x = 0, xSize = parts.getLength(); x < xSize; ++x)
				{
					RotatePart(parts[x][y][z], rotation, center);
				}
			}
		}
		else
		{
			FError::Throwf(TEXT("Calculation error"));
		}
	}

	void CubeParts::RotatePart(PartPtr part, const FRotator& rotation, const FVector& center)
	{
		if (!part)
		{
			return;
		}

		part->AddLocalRotation(rotation);
	}

}; // namespace RC
