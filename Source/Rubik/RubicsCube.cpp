// Fill out your copyright notice in the Description page of Project Settings.

#include "Rubik.h"
#include "RubicsCube.h"
#include "RCRotationCommand.h"


static float progress = 0.0f;
static int currentAxis = 0;
static int currentStep = 0;
static int steps = 0;
static bool front = true;


// Sets default values
ARubicsCube::ARubicsCube(const class FObjectInitializer& OI)
	: Super(OI)
	, GridSize(3)
	, InitialBlockSize(23.0f)
	, InitialSize(80.0f)
	, Type("Standart")
	, parts(3, 3, 3, InitialBlockSize, -FVector(1.0f, 1.0f, 1.0f) * 0.5 * InitialBlockSize * (GridSize - 1))
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = OI.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComponent"));

	InitCube(OI);
}

// Called when the game starts or when spawned
void ARubicsCube::BeginPlay()
{
	Super::BeginPlay();

	progress = 0.0f;
	int currentAxis = 0;
	int currentStep = 0;
}

// Called every frame
void ARubicsCube::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	progress += DeltaTime * 0.5;


	while (progress >= 1.0f || !commandHistory.GetCurrentCommand().IsValid())
	{
		GameBase::Command::Ptr currentCommand = commandHistory.GetCurrentCommand();
		if (currentCommand.IsValid())
		{
			if (front)
			{
				if (steps > 5)
				{
					front = false;
				}
				else
				{
					currentCommand->Execute();
				}
			}
			else
			{
				commandHistory.MoveBackward();

				if (commandHistory.IsOnTail())
				{
					front = true;
					steps = 0;
				}
				else
				{
					commandHistory.GetCurrentCommand()->Unexecute();
				}
			}
		}
		else
		{
			front = true;
		}

		if (front)
		{
			currentStep = FMath::Rand() % GridSize;
			currentAxis = FMath::Rand() % 6;

			RC::RotationAxis axis;

			switch (currentAxis)
			{
			case 0:
				axis = RC::RotationAxis::FX;
				break;
			case 1:
				axis = RC::RotationAxis::FY;
				break;
			case 2:
				axis = RC::RotationAxis::FZ;
				break;
			case 3:
				axis = RC::RotationAxis::RX;
				break;
			case 4:
				axis = RC::RotationAxis::RY;
				break;
			case 5:
				axis = RC::RotationAxis::RZ;
				break;
			default:
				FError::Throwf(TEXT("Unusual axis"));
				break;
			}

			commandHistory.AddCommand(RC::RotationCommand::Create(this, axis, currentStep));
			++steps;
		}

		if (progress >= 1.0f)
		{
			progress -= 1.0f;
		}
	}

	commandHistory.GetCurrentCommand()->SetProgress(front ? progress : 1.0 - progress);
}

void ARubicsCube::InitCube(const class FObjectInitializer& OI)
{
	for (int k = 0; k < GridSize; ++k)
	{
		for (int j = 0; j < GridSize; ++j)
		{
			for (int i = 0; i < GridSize; ++i)
			{
				InitCubePart(OI, RC::CubeParts::Coord(i, j, k));
			}
		}
	}
}

static UStaticMesh * Init1BoardPart()
{

	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshObjFinder(TEXT("StaticMesh'/Game/Cube/1Board/1Board_LowPoly.1Board_LowPoly'"));
	return meshObjFinder.Object;
}

static UStaticMesh * Init2BoardPart()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshObjFinder(TEXT("StaticMesh'/Game/Cube/2Board/2Board_LowPoly.2Board_LowPoly'"));
	return meshObjFinder.Object;
}

static UStaticMesh * Init3BoardPart()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshObjFinder(TEXT("StaticMesh'/Game/Cube/3Board/3Board_LowPoly.3Board_LowPoly'"));
	return meshObjFinder.Object;
}

static UStaticMesh * InitSide()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshObjFinder(TEXT("StaticMesh'/Game/Cube/Side/Side_low.Side_low'"));
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

UStaticMeshComponent * ARubicsCube::ConstructSide(UStaticMesh * staticMesh, UStaticMeshComponent * parentComponent, FName socketName, const class FObjectInitializer& OI, FVector location, FRotator rotation)
{
	UStaticMeshComponent* blockComponent = OI.CreateDefaultSubobject < UStaticMeshComponent >(this, MakeUniqueObjectName(this, AActor::StaticClass(), "Side"));
	blockComponent->SetStaticMesh(staticMesh);
	blockComponent->AttachParent = parentComponent;
	blockComponent->AttachSocketName = socketName;
	blockComponent->SetRelativeLocationAndRotation(location, rotation);
	return blockComponent;
}

UMaterialInstance * ARubicsCube::GetSideMaterial(const RC::CubeParts::Coord& coord, int sideNumber)
{
	int colorIdx = 5;

	if (coord.x == 0 && sideNumber == 0)
	{
		colorIdx = 0;
	}
	else if (coord.x == GridSize - 1 && sideNumber == 0)
	{
		colorIdx = 1;
	}
	else
	{
		colorIdx = 2;
	}

	const TCHAR* colorName;

	switch (colorIdx)
	{
	case 0:
		colorName = TEXT("MaterialInstanceConstant'/Game/Cube/Side/Blue.Blue'");
		break;
	case 1:
		colorName = TEXT("MaterialInstanceConstant'/Game/Cube/Side/Green.Green'");
		break;
	case 2:
		colorName = TEXT("MaterialInstanceConstant'/Game/Cube/Side/Orange.Orange'");
		break;
	case 3:
		colorName = TEXT("MaterialInstanceConstant'/Game/Cube/Side/Red.Red'");
		break;
	case 4:
		colorName = TEXT("MaterialInstanceConstant'/Game/Cube/Side/White.White'");
		break;
	case 5:
		colorName = TEXT("MaterialInstanceConstant'/Game/Cube/Side/Yellow.Yellow'");
		break;
	default:
		return nullptr;
		break;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> Material(colorName);

	if (Material.Object != NULL)
	{
		return (UMaterialInstance*)Material.Object;
	}

	return nullptr;
}

void ARubicsCube::AttachSidesToSockets(UStaticMeshComponent * staticMeshComponent, const class FObjectInitializer& OI, const RC::CubeParts::Coord& coord)
{
	auto socketNames = staticMeshComponent->GetAllSocketNames();
	int sideNumber = 0;
	for (auto sName : socketNames)
	{
		UStaticMesh * side = InitSide();
		UStaticMeshComponent * sideComponent = ConstructSide(side, staticMeshComponent, sName, OI, FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));

		UMaterialInstance * material = GetSideMaterial(coord, sideNumber);
		if (material)
		{
			sideComponent->SetMaterial(0, /*UMaterialInstanceDynamic::Create(*/material/*, this)*/);
		}

		++sideNumber;
	}
}

void ARubicsCube::InitCubePart(const class FObjectInitializer& OI, const RC::CubeParts::Coord& coord)
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

	static int a = 0;
	if (mesh != nullptr)
	{
		UStaticMeshComponent * component = ConstructBlock(mesh, OI, FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
		if (a % 3 == 0)
		{
			AttachSidesToSockets(component, OI, coord);
		}
		parts.InsertPart(component, coord);
		a++;
	}
}
