// Fill out your copyright notice in the Description page of Project Settings.

#include "Rubik.h"
#include "RubicsCube.h"
#include "RCRotationCommand.h"

#include "RubiksBlock_Standart_1Side.h"
#include "RubiksBlock_Standart_2Side.h"
#include "RubiksBlock_Standart_3Side.h"
#include "RubiksBlock_Standart_3Side.h"
#include "RubiksSide_Standart.h"


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
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = OI.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComponent"));
}

// Called when the game starts or when spawned
void ARubicsCube::BeginPlay()
{
	Super::BeginPlay();

	parts = TSharedPtr<RC::CubeParts>(new RC::CubeParts(GridSize, GridSize, GridSize, InitialBlockSize, -FVector(1.0f, 1.0f, 1.0f) * 0.5 * InitialBlockSize * (GridSize - 1)));

	progress = 0.0f;
	int currentAxis = 0;
	int currentStep = 0;

	parts->SetMainLocation(this->GetActorLocation());
	parts->SetMainRotation(this->GetActorRotation());

	InitCube();
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

	UpdateParts();
}

void ARubicsCube::UpdateParts()
{
	parts->SetMainLocation(this->GetActorLocation());
	parts->SetMainRotation(this->GetActorRotation());
}

void ARubicsCube::InitCube()
{
	UWorld* const world = GetWorld();
	if (!world)
	{
		return;
	}

	for (int k = 0; k < GridSize; ++k)
	{
		for (int j = 0; j < GridSize; ++j)
		{
			for (int i = 0; i < GridSize; ++i)
			{
				InitCubePart(world, RC::CubeParts::Coord(i, j, k));
			}
		}
	}
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

void ARubicsCube::AttachSidesToSockets(UWorld * const world, AActor * actor, const RC::CubeParts::Coord& coord)
{
	UStaticMeshComponent* component = dynamic_cast<UStaticMeshComponent*>(actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	TArray<FName> socketNames = component->GetAllSocketNames();
	int sideNumber = 0;
	for (const FName& sName : socketNames)
	{
		AActor * side = world->SpawnActor<ARubiksSide_Standart>(ARubiksSide_Standart::StaticClass());
		side->AttachRootComponentTo(component, sName);

		//static ConstructorHelpers::FObjectFinder <UMaterialInterface> material(TEXT("MaterialInstanceConstant'/Game/Cube/Side/White.White'"));
		//dynamic_cast<UStaticMeshComponent*>(side->GetComponentByClass(UStaticMeshComponent::StaticClass()))->SetMaterial(0, material.Object);

		++sideNumber;
	}
}

void ARubicsCube::InitCubePart(UWorld * const world, const RC::CubeParts::Coord& coord)
{
	AActor * actor = nullptr;

	int blocksCount = 0;

	if (coord.x == 0) ++blocksCount;
	if (coord.x == GridSize - 1) ++blocksCount;
	if (coord.y == 0) ++blocksCount;
	if (coord.y == GridSize - 1) ++blocksCount;
	if (coord.z == 0) ++blocksCount;
	if (coord.z == GridSize - 1) ++blocksCount;

	float halfSize = InitialBlockSize * GridSize;

	switch (blocksCount)
	{
	case 0:
		break;
	case 1:
		actor = world->SpawnActor<ARubiksBlock_Standart_1Side>(ARubiksBlock_Standart_1Side::StaticClass());
		break;
	case 2:
		actor = world->SpawnActor<ARubiksBlock_Standart_2Side>(ARubiksBlock_Standart_2Side::StaticClass());
		break;
	case 3:
		actor = world->SpawnActor<ARubiksBlock_Standart_3Side>(ARubiksBlock_Standart_3Side::StaticClass());
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

	if (actor != nullptr)
	{
		AttachSidesToSockets(world, actor, coord);
		parts->InsertPart(actor, coord);
	}
}
