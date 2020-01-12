// Fill out your copyright notice in the Description page of Project Settings.

#include "Rubik.h"
#include "RubicsCube.h"
#include "RCRotationCommand.h"

#include "RubiksBlock_Standart.h"
#include "RubiksSide_Standart.h"


// Sets default values
ARubicsCube::ARubicsCube(const class FObjectInitializer& OI)
	: Super(OI)
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

	CommandHistory = NewObject<UCommandHistory>();
	CommandHistory->SetReceiver(this);

	CommandProgress = 0.0f;

	CommandHistory->MarkInited();

	IsReady = true;

	OnReady.Broadcast();
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
				InitCubePart(world, Coord(i, j, k));
			}
		}
	}
}

void ARubicsCube::AttachSidesToSockets(UWorld * const world, AActor * actor, const Coord& coord)
{
	UStaticMeshComponent* component = Cast<UStaticMeshComponent>(actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	TArray<FName> socketNames = component->GetAllSocketNames();
	for (const FName& sName : socketNames)
	{
		int SideNum = -1;

		if (sName == "Side1" && coord.z == 0)
		{
			SideNum = 0;
		}
		else if (sName == "Side2" && coord.z == GridSize - 1)
		{
			SideNum = 1;
		}
		else if (sName == "Side3" && coord.y == 0)
		{
			SideNum = 2;
		}
		else if (sName == "Side4" && coord.y == GridSize - 1)
		{
			SideNum = 3;
		}
		else if (sName == "Side5" && coord.x == 0)
		{
			SideNum = 4;
		}
		else if (sName == "Side6" && coord.x == GridSize - 1)
		{
			SideNum = 5;
		}

		if (SideNum >= 0)
		{
			ARubiksSide_Standart* side = world->SpawnActor<ARubiksSide_Standart>(ARubiksSide_Standart::StaticClass());
			side->AttachToComponent(component, FAttachmentTransformRules(EAttachmentRule(), false), sName);
			side->SetInitialSideIndex(SideNum);
			if (ARubiksBlock* block = Cast<ARubiksBlock>(actor))
			{
				block->Sides.Push(side);
			}
			FVector relativeCoord = (FQuat(this->GetActorRotation()).Inverse()).RotateVector(component->GetSocketLocation(sName) - this->GetActorLocation());

			side->SetColorIndex(SideNum);

			if (SideColors.IsValidIndex(SideNum))
			{
				UMaterialInstance* material = SideColors[SideNum];
				Cast<UStaticMeshComponent>(side->GetComponentByClass(UStaticMeshComponent::StaticClass()))->SetMaterial(0, material);
			}
			else
			{
				UE_LOG(LogicalError, Error, TEXT("Wrong side number"));
			}
		}
	}
}

void ARubicsCube::InitCubePart(UWorld * const world, const Coord& coord)
{
	ARubiksBlock_Standart* Part = world->SpawnActor<ARubiksBlock_Standart>(ARubiksBlock_Standart::StaticClass());

	if (Part)
	{
		Parts->InsertPart(Part, coord);
		AttachSidesToSockets(world, Part, coord);
	}
}

void ARubicsCube::AddRotation(const RC::RotationAxis& axis, int layerIndex)
{
	FinishRotation();

	IsMovingFront = true;
	if (!CurrentCommand.IsValid())
	{
		CurrentCommand = RC::RotationCommand::Create(axis, layerIndex);
		CurrentCommand->SetTarget(this);
		CommandHistory->AddCommand(CurrentCommand.ToSharedRef());
	}
}

void ARubicsCube::UpdateSideMaterials()
{
	Parts->ForEachPart([this](Coord /*coord*/, ARubikPart* part)
	{
		ARubiksBlock_Standart* partActor = Cast<ARubiksBlock_Standart>(part);
		if (partActor == nullptr)
		{
			return;
		}

		for (auto side : partActor->Sides)
		{
			if (ARubiksSide_Standart* sideStandart = Cast<ARubiksSide_Standart>(side))
			{
				UMaterialInstance* material = SideColors[sideStandart->GetInitialSideIndex()];
				Cast<UStaticMeshComponent>(side->GetComponentByClass(UStaticMeshComponent::StaticClass()))->SetMaterial(0, material);
			}
		}
	});
}

bool ARubicsCube::CanUndoRotation() const
{
	return !CommandHistory->IsOnTail();
}

void ARubicsCube::UndoRotation()
{
	FinishRotation();

	IsMovingFront = false;
	if (!CurrentCommand.IsValid() && !CommandHistory->IsOnTail())
	{
		CurrentCommand = CommandHistory->GetPrevCommand();
		CurrentCommand->Unexecute();
		CommandHistory->MoveBackward();
	}
	OnMoveDone.Broadcast();
}

bool ARubicsCube::CanRedoRotation() const
{
	return !CommandHistory->IsOnHead();
}

void ARubicsCube::RedoRotation()
{
	FinishRotation();

	IsMovingFront = true;
	if (!CurrentCommand.IsValid() && !CommandHistory->IsOnHead())
	{
		CurrentCommand = CommandHistory->GetNextCommand();
		CommandHistory->MoveForward();
	}

	OnMoveDone.Broadcast();
}

UCommandHistory* ARubicsCube::GetHistory()
{
	return CommandHistory;
}

void ARubicsCube::SetHistory(UCommandHistory* NewHistory)
{
	if (NewHistory && NewHistory->IsValidLowLevel())
	{
		if (CommandHistory && CommandHistory->IsValidLowLevel())
		{
			CommandHistory->SetReceiver(nullptr);
		}

		CommandHistory = NewHistory;
		CommandHistory->SetReceiver(this);
	}
}

void ARubicsCube::OnHistoryLoaded()
{
	OnMoveDone.Broadcast();
}

void ARubicsCube::MakeRandomMoves(int Count)
{
	int PreviousAxisIndexReversed = -1;
	int PreviousLayerIndex = -1;
	int i = 0;
	while (i < Count)
	{
		int AxisIndex = FMath::Rand() % 6;
		int LayerIndex = FMath::Rand() % GridSize;

		// don't produce a rotation if it negates the previous one
		if (LayerIndex == PreviousAxisIndexReversed && PreviousLayerIndex == LayerIndex)
		{
			continue;
		}

		RC::RotationAxis Axis;
		switch (AxisIndex)
		{
		case 0:
			Axis = RC::RotationAxis::FX;
			break;
		case 1:
			Axis = RC::RotationAxis::FY;
			break;
		case 2:
			Axis = RC::RotationAxis::FZ;
			break;
		case 3:
			Axis = RC::RotationAxis::RX;
			break;
		case 4:
			Axis = RC::RotationAxis::RY;
			break;
		case 5:
			Axis = RC::RotationAxis::RZ;
			break;
		default:
			return;
		}

		AddRotation(Axis, LayerIndex);

		PreviousAxisIndexReversed = (LayerIndex + 3) % 6;
		PreviousLayerIndex = LayerIndex;
		++i;
	}
	OnMoveDone.Broadcast();
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
	}
	else
	{
		CurrentCommand->SetProgress(0.0f);
	}

	CurrentCommand = nullptr;
	CommandProgress = 0.0f;

	if (Parts->IsAssembled())
	{
		OnAssembled.Broadcast();
	}
}
