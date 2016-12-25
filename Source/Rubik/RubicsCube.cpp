// Fill out your copyright notice in the Description page of Project Settings.

#include "Rubik.h"
#include "RubicsCube.h"
#include "RCRotationCommand.h"

#include "RubiksBlock_Standart.h"
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
	, CommandHistory(this)
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

	//RubiksGameState* gameState =  Cast<RubiksGameState>(GetWorld()->GameState);
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

UMaterialInstanceConstant * ARubicsCube::GetSideMaterial(int sideNum)
{
	switch (sideNum) {
	case 0:
		return SideColor1;
	case 1:
		return SideColor2;
	case 2:
		return SideColor3;
	case 3:
		return SideColor4;
	case 4:
		return SideColor5;
	case 5:
		return SideColor6;
	default:
		UE_LOG(LogicalError, Error, TEXT("Wrong side number"));
		return SideColor1;
	}
}

void ARubicsCube::AttachSidesToSockets(UWorld * const world, AActor * actor, const RC::CubeParts::Coord& coord)
{
	UStaticMeshComponent* component = Cast<UStaticMeshComponent>(actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	TArray<FName> socketNames = component->GetAllSocketNames();
	for (const FName& sName : socketNames)
	{
		int SideNum = -1;

		if (sName == "Side1" && coord.z == GridSize - 1)
		{
			SideNum = 0;
		}
		else if (sName == "Side2" && coord.x == GridSize - 1)
		{
			SideNum = 1;
		}
		else if (sName == "Side3" && coord.y == 0)
		{
			SideNum = 2;
		}
		else if (sName == "Side4" && coord.z == 0)
		{
			SideNum = 3;
		}
		else if (sName == "Side5" && coord.x == 0)
		{
			SideNum = 4;
		}
		else if (sName == "Side6" && coord.y == GridSize - 1)
		{
			SideNum = 5;
		}

		if (SideNum >= 0)
		{
			AActor * side = world->SpawnActor<ARubiksSide_Standart>(ARubiksSide_Standart::StaticClass());
			side->AttachToComponent(component, FAttachmentTransformRules(EAttachmentRule(), false), sName);
			Cast<ARubiksBlock>(actor)->Sides.Push(side);
			FVector relativeCoord = (FQuat(this->GetActorRotation()).Inverse()).RotateVector(component->GetSocketLocation(sName) - this->GetActorLocation());

			UMaterialInstanceConstant * material = GetSideMaterial(SideNum);
			Cast<UStaticMeshComponent>(side->GetComponentByClass(UStaticMeshComponent::StaticClass()))->SetMaterial(0, material);
		}
	}
}

void ARubicsCube::InitCubePart(UWorld * const world, const RC::CubeParts::Coord& coord)
{
	AActor * actor = world->SpawnActor<ARubiksBlock_Standart>(ARubiksBlock_Standart::StaticClass());

	if (actor != nullptr)
	{
		Parts->InsertPart(actor, coord);
		AttachSidesToSockets(world, actor, coord);
	}
}

bool ARubicsCube::AddRotation(const RC::RotationAxis& axis, int layerIndex)
{
	FinishRotation();

	IsMovingFront = true;
	if (!CurrentCommand.IsValid())
	{
		CommandHistory.ClearNextCommands();
		CurrentCommand = RC::RotationCommand::Create(axis, layerIndex);
		CurrentCommand->SetTarget(this);
		return true;
	}
	return false;
}

TSharedPtr<FJsonObject> ARubicsCube::GetCommandsAsJson()
{
	return CommandHistory.ToJson();
}

void ARubicsCube::LoadCommandsFromJson(TSharedPtr<FJsonObject> serialized)
{
	CommandHistory.LoadFromJson(serialized);
}

void ARubicsCube::UndoRotation()
{
	FinishRotation();

	IsMovingFront = false;
	if (!CurrentCommand.IsValid() && !CommandHistory.IsOnTail())
	{
		CurrentCommand = CommandHistory.GetPrevCommand();
		CurrentCommand->Unexecute();
		CommandHistory.MoveBackward();
	}
}

void ARubicsCube::RedoRotation()
{
	FinishRotation();

	IsMovingFront = true;
	if (!CurrentCommand.IsValid() && !CommandHistory.IsOnHead())
	{
		CurrentCommand = CommandHistory.GetNextCommand();
	}
}

void ARubicsCube::FinishRotation()
{
	if (!CurrentCommand.IsValid())
	{
		return;
	}

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
