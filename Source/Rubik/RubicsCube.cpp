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
	, IsNeedUpdateParts(false)
	, RotationSpeed(10.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = OI.CreateDefaultSubobject<USceneComponent>(this, TEXT("SceneComponent"));
}

// Called when the game starts or when spawned
void ARubicsCube::BeginPlay()
{
	Super::BeginPlay();

	Parts = TSharedPtr<RC::CubeParts>(new RC::CubeParts(GridSize, GridSize, GridSize, InitialBlockSize, -FVector(1.0f, 1.0f, 1.0f) * 0.5 * InitialBlockSize * (GridSize - 1)));

	Parts->SetMainLocation(this->GetActorLocation());
	Parts->SetMainRotation(FQuat(this->GetActorRotation()));

	InitCube();

	CommandProgress = 0.0f;
}

// Called every frame
void ARubicsCube::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);

	UpdateParts();

	if (CurrentCommand.IsValid())
	{
		CommandProgress += DeltaTime * RotationSpeed;
		if (CommandProgress < 1.0f)
		{
			CurrentCommand->SetProgress(IsMovingFront ? CommandProgress : 1.0f - CommandProgress);
		}
		else
		{
			FinishRotation();
		}
	}
}

void ARubicsCube::UpdateParts()
{
	if (IsNeedUpdateParts)
	{
		Parts->SetMainLocation(this->GetActorLocation());
		Parts->SetMainRotation(FQuat(this->GetActorRotation()));
		Parts->UpdateAllParts();
		IsNeedUpdateParts = false;
	}
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
	for (const FName& sName : socketNames)
	{
		AActor * side = world->SpawnActor<ARubiksSide_Standart>(ARubiksSide_Standart::StaticClass());
		side->AttachRootComponentTo(component, sName);
		dynamic_cast<ARubiksBlock*>(actor)->Sides.Push(side);
		FVector relativeCoord = (FQuat(this->GetActorRotation()).Inverse()).RotateVector(component->GetSocketLocation(sName) - this->GetActorLocation());

		UMaterialInstanceConstant * material = GetSideMaterial(relativeCoord);
		dynamic_cast<UStaticMeshComponent*>(side->GetComponentByClass(UStaticMeshComponent::StaticClass()))->SetMaterial(0, material);
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
		Parts->InsertPart(actor, coord);
		AttachSidesToSockets(world, actor, coord);
	}
}

bool ARubicsCube::AddRotation(const RC::RotationAxis& axis, int layerIndex)
{
	IsMovingFront = true;
	if (!CurrentCommand.IsValid())
	{
		CommandHistory.ClearNextCommands();
		CurrentCommand = RC::RotationCommand::Create(this, axis, layerIndex);
		return true;
	}
	return false;
}

void ARubicsCube::UndoRotation()
{
	IsMovingFront = false;
	if (!CurrentCommand.IsValid() && !CommandHistory.IsOnTail())
	{
		CurrentCommand = CommandHistory.GetPrewCommand();
		CurrentCommand->Unexecute();
		CommandHistory.MoveBackward();
	}
}

void ARubicsCube::RedoRotation()
{
	IsMovingFront = true;
	if (!CurrentCommand.IsValid() && !CommandHistory.IsOnHead())
	{
		CurrentCommand = CommandHistory.GetNextCommand();
	}
}

void ARubicsCube::FinishRotation()
{
	if (IsMovingFront)
	{
		CurrentCommand->Execute();
		if (CommandHistory.IsOnHead())
		{
			CommandHistory.AddCommand(CurrentCommand.ToSharedRef());
		}
		else
		{
			CommandHistory.MoveForward();
		}
	}
	else
	{
		CurrentCommand->SetProgress(0.0f);
	}

	CurrentCommand = nullptr;
	CommandProgress = 0.0f;
}
