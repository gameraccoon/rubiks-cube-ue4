// Fill out your copyright notice in the Description page of Project Settings.

#include "Rubik.h"
#include "RubicsCube.h"
#include "RCRotationCommand.h"

#include "RubiksBlock_Standart_1Side.h"
#include "RubiksBlock_Standart_2Side.h"
#include "RubiksBlock_Standart_3Side.h"
#include "RubiksBlock_Standart_3Side.h"
#include "RubiksSide_Standart.h"


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

	progress = 0.0f;
	currentAxis = 0;
	currentStep = 0;
	steps = 0;
	front = true;
}

// Called every frame
void ARubicsCube::Tick( float DeltaTime )
{
	const int COMMANDS_SIZE = 5;
	Super::Tick( DeltaTime );

	progress += DeltaTime * 0.5;

	while (progress >= 1.0f || !currentCommand.IsValid())
	{
		if (currentCommand.IsValid())
		{
			if (front)
			{
				if (steps < COMMANDS_SIZE)
				{
					currentCommand->Execute();
					commandHistory.AddCommand(currentCommand.ToSharedRef());
				}
				else
				{
					front = false;
				}
			}
			else
			{
				currentCommand = commandHistory.GetPrewCommand();

				if (currentCommand.IsValid())
				{
					commandHistory.MoveBackward();
					currentCommand->Unexecute();
				}
				else
				{
					front = true;
					steps = 0;
				}
			}
		}
		else
		{
			front = true;
		}

		// add new command
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
			currentCommand = RC::RotationCommand::Create(this, axis, currentStep);
			++steps;
		}

		if (progress >= 1.0f)
		{
			progress -= 1.0f;
		}
	}

	currentCommand->SetProgress(front ? progress : 1.0 - progress);

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

UMaterialInstanceConstant * ARubicsCube::GetSideMaterial(const FVector& sidePos)
{
	float sideOffset = InitialBlockSize * ((GridSize - 1) * 0.6);
	if (sidePos.X > sideOffset)
	{
		return SideColor1;
	}
	else if (sidePos.X < -sideOffset)
	{
		return SideColor2;
	}
	else if (sidePos.Y > sideOffset)
	{
		return SideColor3;
	}
	else if (sidePos.Y < -sideOffset)
	{
		return SideColor4;
	}
	else if (sidePos.Z > sideOffset)
	{
		return SideColor5;
	}
	else if (sidePos.Z < -sideOffset)
	{
		return SideColor6;
	}
	else
	{
		FError::Throwf(TEXT("Wrong side location"));
		return SideColor6;
	}
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
		FVector relativeCoord = (-this->GetActorRotation()).RotateVector(component->GetSocketLocation(sName) - this->GetActorLocation());

		UMaterialInstanceConstant * material = GetSideMaterial(relativeCoord);
		dynamic_cast<UStaticMeshComponent*>(side->GetComponentByClass(UStaticMeshComponent::StaticClass()))->SetMaterial(0, material);

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
		parts->InsertPart(actor, coord);
		AttachSidesToSockets(world, actor, coord);
	}
}
