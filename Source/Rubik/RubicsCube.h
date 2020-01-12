// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Base/CommandHistory.h"
#include "RCParts.h"
#include "RubicsCube.generated.h"

namespace RC {
	class CubeCommand;
	class RotationCommand;
}

UCLASS()
class RUBIK_API ARubicsCube
	: public AActor
{
	GENERATED_BODY()
public:

	/** size in elements */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters", meta = (ClampMin = "2", ClampMax = "5", UIMin = "2", UIMax = "5"))
	uint8 GridSize = 3;

	/** single block size in units */
	UPROPERTY(EditAnywhere, Category = "Parameters")
	float BlockSize = 23.0f;

	/** cube's type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	FName Type = TEXT("Standart");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
	TArray<UMaterialInstance*> SideColors;

	UFUNCTION(BlueprintCallable)
	void InitCube(uint8 InGridSize);

	UFUNCTION(BlueprintCallable)
	void ClearCube();

	UFUNCTION(BlueprintCallable, Category = "Colors")
	void UpdateSideMaterials();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool CanUndoRotation() const;

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void UndoRotation();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool CanRedoRotation() const;

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void RedoRotation();

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	UCommandHistory* GetHistory();

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	void SetHistory(UCommandHistory* NewHistory);

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	void OnHistoryLoaded();

	UPROPERTY(BlueprintReadOnly)
	bool IsReady = false;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	float RotationSpeed = 10.0f;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleMulticastDelegate);

	UPROPERTY(BlueprintAssignable)
	FSimpleMulticastDelegate OnReady;

	UPROPERTY(BlueprintAssignable)
	FSimpleMulticastDelegate OnAssembled;

	UPROPERTY(BlueprintAssignable)
	FSimpleMulticastDelegate OnMoveDone;

	UFUNCTION(BlueprintCallable, Category = "Randomize")
	void MakeRandomMoves(int Count);

public:
	/** Sets default values for this actor's properties */
	ARubicsCube(const class FObjectInitializer& OI);

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;
	
	/** Called every frame */
	virtual void Tick(float DeltaSeconds) override;

	inline void ScheduleUpdateParts() { IsNeedUpdateParts = true; }

	void AddRotation(const RC::RotationAxis& axis, int layerIndex);

private:

	UPROPERTY()
	UCommandHistory* CommandHistory = nullptr;

	Command::Ptr CurrentCommand;
	float CommandProgress = 0.0f;

	TSharedPtr<RC::CubeParts> Parts;

	bool IsNeedUpdateParts = false;
	bool IsMovingFront = true;

private:
	void InitCubePart(UWorld * const world, const Coord& coord);

	void AttachSidesToSockets(UWorld * const world, AActor * actor, const Coord& coord);

	void UpdateParts();

	void FinishRotation();

	friend class RC::CubeCommand;
	friend class RC::RotationCommand;

};
