// Fill out your copyright notice in the Description page of Project Settings.

#include "Rubik.h"
#include "RubicsCube.h"

enum class RotationAxis {
	FX, RX,
	FY, RY,
	FZ, RZ
};

/// some abstract action with Rubik's cube
class RubiksCubeCommand
	: public Command
{
public:
	RubiksCubeCommand(ARubicsCube * target)
		: target(target)
	{}

protected:
	/// get target for modification
	ARubicsCube * GetTarget() { return target; }

private:
	/// pointer to object that we will modify
	ARubicsCube * const target;
};

/// Rotation one of layers of a Rubik's cube
class RotationCommand
	: public RubiksCubeCommand
{
public:
	static Ref Create(ARubicsCube * target, RotationAxis axis, int layerIdx)
	{
		return Ref(new RotationCommand(target, axis, layerIdx));
	}

	~RotationCommand() {}

	virtual void Execute() override
	{
		if (isExecuted)
		{
			FError::Throwf(TEXT("Try to execute already executed command"));
			return;
		}

		isExecuted = true;
	}

	virtual void Unexecute() override
	{
		if (!isExecuted)
		{
			FError::Throwf(TEXT("Try to unexecute not executed command"));
			return;
		}

		isExecuted = false;
	}

	virtual bool IsContinious() override
	{
		return true;
	}

	virtual void SetProgress(float progress) override
	{
		// ToDo: implementation
	}

private:
	RotationCommand(ARubicsCube * target, RotationAxis axis, int layerIdx)
		: RubiksCubeCommand(target)
		, isExecuted(false)
		, axis(axis)
		, layerIndex(layerIdx)
	{}

private:
	/// have command been already executed
	bool isExecuted;
	/// axis we rotating around (directed)
	const RotationAxis axis;
	/// index of layer that we rotating
	const int layerIndex;
};

// Sets default values
ARubicsCube::ARubicsCube(const class FObjectInitializer& OI)
	: Super(OI)
	, GridSize(3)
	, InitialBlockSize(23.f)
	, InitialSize(80.0f)
	, Type("Standart")
	, commandsHead(commandsHistory.GetTail())
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = OI.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComponent"));

	InitCube(OI);
}

// Called when the game starts or when spawned
void ARubicsCube::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARubicsCube::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ARubicsCube::InitCube(const class FObjectInitializer& OI)
{
	for (int k = 0; k < GridSize; ++k)
	{
		for (int j = 0; j < GridSize; ++j)
		{
			for (int i = 0; i < GridSize; ++i)
			{
				InitCubePart(OI, CubePart::Coord(i, j, k));
			}
		}
	}
}

UStaticMesh * ARubicsCube::Init1BoardPart()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshObjFinder(TEXT("StaticMesh'/Game/Cube/1Board/1Board_LowPoly.1Board_LowPoly'"));
	return meshObjFinder.Object;
}

UStaticMesh * ARubicsCube::Init2BoardPart()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshObjFinder(TEXT("StaticMesh'/Game/Cube/2Board/2Board_LowPoly.2Board_LowPoly'"));
	return meshObjFinder.Object;
}

UStaticMesh * ARubicsCube::Init3BoardPart()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshObjFinder(TEXT("StaticMesh'/Game/Cube/3Board/3Board_LowPoly.3Board_LowPoly'"));
	return meshObjFinder.Object;
}

UStaticMeshComponent * ARubicsCube::ConstructBlock(UStaticMesh * staticMesh, const class FObjectInitializer& OI, FVector location, FRotator rotation)
{
	UStaticMeshComponent* blockComponent = OI.CreateDefaultSubobject < UStaticMeshComponent >(this, MakeUniqueObjectName(this, AActor::StaticClass(), "Block"));
	blockComponent->SetStaticMesh(staticMesh);
	blockComponent->AttachParent = RootComponent;
	blockComponent->SetRelativeLocationAndRotation(location, rotation);
	return blockComponent;
}

void ARubicsCube::InitCubePart(const class FObjectInitializer& OI, const CubePart::Coord& coord)
{
	UStaticMesh * mesh = nullptr;

	int blocksCount = 0;

	if (coord.x == 0) ++blocksCount;
	if (coord.x == GridSize - 1) ++blocksCount;
	if (coord.y == 0) ++blocksCount;
	if (coord.y == GridSize - 1) ++blocksCount;
	if (coord.z == 0) ++blocksCount;
	if (coord.z == GridSize - 1) ++blocksCount;

	switch (blocksCount)
	{
	case 0:
		break;
	case 1:
		mesh = Init1BoardPart();
		break;
	case 2:
		mesh = Init2BoardPart();
		break;
	case 3:
		mesh = Init3BoardPart();
		break;
	case 4:
	case 5:
	case 6:
		FError::Throwf(TEXT("Unusual cube form"));
		break;
	default:
		FError::Throwf(TEXT("Error with cube form calculationg"));
		break;
	}

	if (mesh != nullptr)
	{
		cubeParts.Push(ConstructBlock(mesh, OI, FVector(coord.x, coord.y, coord.z) * InitialBlockSize, InitBlockRotation(coord)));
	}
}

// ToDo: find any better way to calculate rotation
FRotator ARubicsCube::InitBlockRotation(const CubePart::Coord& coord)
{
	if (coord.x == 0)
	{
		if (coord.y == 0)
		{
			if (coord.z == 0)
			{
				return FRotator(0.0f, -90.0f, -90.0f);
			}
			else if (coord.z == GridSize - 1)
			{
				return FRotator(0.0f, -90.0f, 0.0f);
			}

			return FRotator(-90.0f, -90.0f, 0.0f);
		}
		else if (coord.y == GridSize - 1)
		{
			if (coord.z == 0)
			{
				return FRotator(0.0f, 180.0f, -90.0f);
			}
			else if (coord.z == GridSize - 1)
			{
				return FRotator(0.0f, 180.0f, 0.0f);
			}

			return FRotator(-90.0f, 180.0f, 0.0f);
		}

		if (coord.z == 0)
		{
			return FRotator(0.0f, -90.0f, -90.0f);
		}
		else if (coord.z == GridSize - 1)
		{
			return FRotator(0.0f, -90.0f, 0.0f);
		}

		return FRotator(90.0f, 0.0f, 0.0f);
	}
	else if (coord.x == GridSize - 1)
	{
		if (coord.y == 0)
		{
			if (coord.z == 0)
			{
				return FRotator(0.0f, 0.0f, -90.0f);
			}
			else if (coord.z == GridSize - 1)
			{
				return FRotator(0.0f, 0.0f, 0.0f);
			}

			return FRotator(-90.0f, 0.0f, 0.0f);
		}
		else if (coord.y == GridSize - 1)
		{
			if (coord.z == 0)
			{
				return FRotator(0.0f, 0.0f, 180.0f);
			}
			else if (coord.z == GridSize - 1)
			{
				return FRotator(0.0f, 90.0f, 0.0f);
			}

			return FRotator(-90.0f, 90.0f, 0.0f);
		}

		if (coord.z == 0)
		{
			return FRotator(0.0f, 90.0f, -90.0f);
		}
		else if (coord.z == GridSize - 1)
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
		else if (coord.z == GridSize - 1)
		{
			return FRotator(0.0f, 0.0f, 0.0f);
		}

		return FRotator(0.0f, 0.0f, -90.0f);
	}
	else if (coord.y == GridSize - 1)
	{
		if (coord.z == 0)
		{
			return FRotator(0.0f, 0.0f, 180.0f);
		}
		else if (coord.z == GridSize - 1)
		{
			return FRotator(0.0f, 0.0f, 90.0f);
		}

		return FRotator(0.0f, 0.0f, 90.0f);
	}
	
	if (coord.z == 0)
	{
		return FRotator(180.0f, 0.0f, 0.0f);
	}
	else if (coord.z == GridSize - 1)
	{
		return FRotator(0.0f, 0.0f, 0.0f);
	}

	FError::Throwf(TEXT("Wrong actor position to rotate"));
	return FRotator::ZeroRotator;
}
