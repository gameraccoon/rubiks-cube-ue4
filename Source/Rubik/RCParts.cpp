#include "Rubik.h"
#include "RubiksBlock.h"
#include "RubiksSide_Standart.h"
#include "RCParts.h"

namespace RC
{
	CubeParts::CubeParts(int length, int width, int height, float _initialBlockSize, FVector _centerShift)
		: parts(length, width, height, PartInfo())
		, initialBlockSize(_initialBlockSize)
		, centerShift(_centerShift)
		, mainLocation(0.0f, 0.0f, 0.0f)
		, mainRotation(FRotator(0.0f, 0.0f, 0.0f))
	{
	}

	void CubeParts::InsertPart(ARubikPart* part, Coord pos)
	{
		PartInfo& partInfo = parts[pos.x][pos.y][pos.z];

		if (partInfo.ptr)
		{
			UE_LOG(LogicalError, Error, TEXT("Reassignation of cube part"));
		}

		if (!part)
		{
			UE_LOG(LogicalError, Error, TEXT("Part is NULL"));
			return;
		}

		FVector location(FVector(pos.x, pos.y, pos.z) * initialBlockSize);
		FQuat roataion(GetPartInitialRotation(pos));

		part->SetActorLocation(mainLocation + mainRotation.RotateVector(location + centerShift));
		part->SetActorRotation((mainRotation * roataion).Rotator());
		part->InitialPos = pos;

		partInfo.ptr = part;
		partInfo.localLocation = location + centerShift;
		partInfo.localRotation = roataion;
		partInfo.baseLocation = partInfo.localLocation;
		partInfo.baseRotation = partInfo.localRotation;
	}

	void CubeParts::RotateSlice(RotationAxis axis, int pos, float angle)
	{
		Slice slice = GetSlice(axis, pos);
		RotateSlice(slice, axis, angle);
	}

	void CubeParts::RotateSlice(Slice& slice, RotationAxis axis, float angle)
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

		for (unsigned int y = 0, ySize = slice.getWidth(); y < ySize; ++y)
		{
			for (unsigned int x = 0, xSize = slice.getLength(); x < xSize; ++x)
			{
				RotatePart(slice[x][y], rotation);
			}
		}
	}

	void CubeParts::MakeSnapshot(Slice& slice)
	{
		for (unsigned int y = 0, ySize = slice.getWidth(); y < ySize; ++y)
		{
			for (unsigned int x = 0, xSize = slice.getLength(); x < xSize; ++x)
			{
				PartInfo *partinfo = slice[x][y];
				if (partinfo->ptr)
				{
					partinfo->baseLocation = partinfo->localLocation;
					partinfo->baseRotation = partinfo->localRotation;
				}
			}
		}
	}

	void CubeParts::RenewPartsLocations(RotationAxis axis, int pos)
	{
		Slice slice = GetSlice(axis, pos);

		RotateSlice(slice, axis, 90.0f);
		MakeSnapshot(slice);

		const int N = (int) slice.getWidth();

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

		RearrangeControls();
	}

	void CubeParts::RotatePart(PartInfo* part, const FRotator& rotation)
	{
		if (!part->ptr)
		{
			return;
		}

		FQuat rotationQuat(rotation);
		part->localLocation = rotationQuat.RotateVector(part->baseLocation);
		part->localRotation = rotationQuat * part->baseRotation;

		part->ptr->SetActorLocation(mainLocation + mainRotation.RotateVector(part->localLocation));
		part->ptr->SetActorRotation((mainRotation * part->localRotation).Rotator());
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
			UE_LOG(LogicalError, Error, TEXT("Calculation error"));
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
			UE_LOG(LogicalError, Error, TEXT("Calculation error"));
		}

		return slice;
	}

	void CubeParts::UpdateAllParts()
	{
		for (unsigned int z = 0, zSize = parts.getHeight(); z < zSize; ++z)
		{
			for (unsigned int y = 0, ySize = parts.getWidth(); y < ySize; ++y)
			{
				for (unsigned int x = 0, xSize = parts.getLength(); x < xSize; ++x)
				{
					PartInfo &part = parts[x][y][z];
					if (part.ptr)
					{
						part.localLocation = part.baseLocation;
						part.localRotation = part.baseRotation;

						part.ptr->SetActorLocation(mainLocation + mainRotation.RotateVector(part.localLocation));
						part.ptr->SetActorRotation((mainRotation * part.localRotation).Rotator());
					}
				}
			}
		}

		RearrangeControls();
	}

	inline bool IsBlockHaveValidColors(TArray<int>& InOutSideColors, const int sidesCount, const ARubiksBlock* block)
	{
		for (const auto side : block->Sides)
		{
			if (const ARubiksSide_Standart* rubiksSide = Cast<ARubiksSide_Standart>(side))
			{
				int sideIndex = rubiksSide->GetSideIndex();
				int colorIndex = rubiksSide->GetColorIndex();
				if (sideIndex >= 0 && sideIndex < sidesCount) {

					if (InOutSideColors[sideIndex] == -1)
					{
						InOutSideColors[sideIndex] = colorIndex;
					}

					if (InOutSideColors[sideIndex] != colorIndex)
					{
						return false;
					}
				}
			}
		}
		return true;
	}

	bool CubeParts::IsAssembled()
	{
		const int sidesCount = 6;
		TArray<int> SideColors;
		SideColors.SetNum(sidesCount);
		for (int i = 0; i < sidesCount; ++i)
		{
			SideColors[i] = -1;
		}

		for (unsigned int z = 0, zSize = parts.getHeight(); z < zSize; ++z)
		{
			for (unsigned int y = 0, ySize = parts.getWidth(); y < ySize; ++y)
			{
				for (unsigned int x = 0, xSize = parts.getLength(); x < xSize; ++x)
				{
					if (ARubiksBlock* block = Cast<ARubiksBlock>(parts[x][y][z].ptr))
					{
						bool isColorValid = IsBlockHaveValidColors(SideColors, sidesCount, block);
						if (!isColorValid)
						{
							return false;
						}
					}
				}
			}
		}

		return true;
	}

	void CubeParts::RearrangeControls()
	{
		for (unsigned int z = 0, zSize = parts.getHeight(); z < zSize; ++z)
		{
			for (unsigned int y = 0, ySize = parts.getWidth(); y < ySize; ++y)
			{
				for (unsigned int x = 0, xSize = parts.getLength(); x < xSize; ++x)
				{
					PartInfo &part = parts[x][y][z];
					if (part.ptr)
					{
						RearrangePartControls(part.ptr, Coord(x, y, z));
					}
				}
			}
		}
	}

	void CubeParts::RearrangeSideControls(AActor * side, const Coord& coord)
	{
		const FVector sidePos = (mainRotation.Inverse()).RotateVector(side->GetActorLocation() - mainLocation);
		const float sideOffset = initialBlockSize * ((parts.getLength() - 1) * 0.6);
		
		TArray<RC::MovementDirection> directions;
		int sideIndex = -1;

		if (sidePos.X > sideOffset)
		{
			directions.Push(RC::MovementDirection(FVector(0.0f, 0.0f, 1.0f), RC::RotationAxis::FY, coord.y));
			directions.Push(RC::MovementDirection(FVector(0.0f, 0.f, -1.0f), RC::RotationAxis::RY, parts.getWidth() - coord.y - 1));
			directions.Push(RC::MovementDirection(FVector(0.0f, 1.0f, 0.0f), RC::RotationAxis::FZ, coord.z));
			directions.Push(RC::MovementDirection(FVector(0.0f, -1.0f, 0.0f), RC::RotationAxis::RZ, parts.getWidth() - coord.z - 1));
			sideIndex = 0;
		}
		else if (sidePos.X < -sideOffset)
		{
			directions.Push(RC::MovementDirection(FVector(0.0f, 0.f, 1.0f), RC::RotationAxis::RY, parts.getWidth() - coord.y - 1));
			directions.Push(RC::MovementDirection(FVector(0.0f, 0.0f, -1.0f), RC::RotationAxis::FY, coord.y));
			directions.Push(RC::MovementDirection(FVector(0.0f, 1.0f, 0.0f), RC::RotationAxis::RZ, parts.getWidth() - coord.z - 1));
			directions.Push(RC::MovementDirection(FVector(0.0f, -1.0f, 0.0f), RC::RotationAxis::FZ, coord.z));
			sideIndex = 1;
		}
		else if (sidePos.Y > sideOffset)
		{
			directions.Push(RC::MovementDirection(FVector(0.0f, 0.0f, 1.0f), RC::RotationAxis::RX, parts.getWidth() - coord.x - 1));
			directions.Push(RC::MovementDirection(FVector(0.0f, 0.0f, -1.0f), RC::RotationAxis::FX, coord.x));
			directions.Push(RC::MovementDirection(FVector(1.0f, 0.0f, 0.0f), RC::RotationAxis::RZ, parts.getWidth() - coord.z - 1));
			directions.Push(RC::MovementDirection(FVector(-1.0f, 0.0f, 0.0f), RC::RotationAxis::FZ, coord.z));
			sideIndex = 2;
		}
		else if (sidePos.Y < -sideOffset)
		{
			directions.Push(RC::MovementDirection(FVector(0.0f, 0.0f, 1.0f), RC::RotationAxis::FX, coord.x));
			directions.Push(RC::MovementDirection(FVector(0.0f, 0.0f, -1.0f), RC::RotationAxis::RX, parts.getWidth() - coord.x - 1));
			directions.Push(RC::MovementDirection(FVector(1.0f, 0.0f, 0.0f), RC::RotationAxis::FZ, coord.z));
			directions.Push(RC::MovementDirection(FVector(-1.0f, 0.0f, 0.0f), RC::RotationAxis::RZ, parts.getWidth() - coord.z - 1));
			sideIndex = 3;
		}
		else if (sidePos.Z > sideOffset)
		{
			directions.Push(RC::MovementDirection(FVector(0.0f, 1.0f, 0.0f), RC::RotationAxis::FX, coord.x));
			directions.Push(RC::MovementDirection(FVector(0.0f, -1.0f, 0.0f), RC::RotationAxis::RX, parts.getWidth() - coord.x - 1));
			directions.Push(RC::MovementDirection(FVector(1.0f, 0.0f, 0.0f), RC::RotationAxis::RY, parts.getWidth() - coord.y - 1));
			directions.Push(RC::MovementDirection(FVector(-1.0f, 0.0f, 0.0f), RC::RotationAxis::FY, coord.y));
			sideIndex = 4;
		}
		else if (sidePos.Z < -sideOffset)
		{
			directions.Push(RC::MovementDirection(FVector(0.0f, 1.0f, 0.0f), RC::RotationAxis::RX, parts.getWidth() - coord.x - 1));
			directions.Push(RC::MovementDirection(FVector(0.0f, -1.0f, 0.0f), RC::RotationAxis::FX, coord.x));
			directions.Push(RC::MovementDirection(FVector(1.0f, 0.0f, 0.0f), RC::RotationAxis::FY, coord.y));
			directions.Push(RC::MovementDirection(FVector(-1.0f, 0.0f, 0.0f), RC::RotationAxis::RY, parts.getWidth() - coord.y - 1));
			sideIndex = 5;
		}
		else
		{
			UE_LOG(LogicalError, Error, TEXT("Unusual side location"));
		}

		if (ARubiksSide_Standart* castedSide = dynamic_cast<ARubiksSide_Standart*>(side))
		{
			castedSide->SetDirections(directions);
			castedSide->SetCurrentSideIndex(sideIndex);
		}
	}

	void CubeParts::RearrangePartControls(AActor * part, const Coord& coord)
	{
		TArray<AActor*>& sides = dynamic_cast<ARubiksBlock*>(part)->Sides;
		for (AActor* side : sides)
		{
			RearrangeSideControls(side, coord);
		}
	}

	FRotator CubeParts::GetPartInitialRotation(const Coord& coord)
	{
		return FRotator::ZeroRotator;
	}

}; // namespace RC
