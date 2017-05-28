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
	UPROPERTY(EditAnywhere, Category = "Parameters", meta = (ClampMin = "2", ClampMax = "5", UIMin = "2", UIMax = "5"))
	uint8 GridSize;

	/** whole cube size in units */
	UPROPERTY(EditAnywhere, Category = "Parameters")
	float InitialSize;

	/** single block size in units */
	UPROPERTY(EditAnywhere, Category = "Parameters")
	float InitialBlockSize;

	/** cubic's type */
	UPROPERTY(EditAnywhere, Category = "Parameters")
	FName Type;

	UPROPERTY(EditAnywhere, Category = "Colors")
	UMaterialInstanceConstant * SideColor1;

	UPROPERTY(EditAnywhere, Category = "Colors")
	UMaterialInstanceConstant * SideColor2;

	UPROPERTY(EditAnywhere, Category = "Colors")
	UMaterialInstanceConstant * SideColor3;

	UPROPERTY(EditAnywhere, Category = "Colors")
	UMaterialInstanceConstant * SideColor4;

	UPROPERTY(EditAnywhere, Category = "Colors")
	UMaterialInstanceConstant * SideColor5;

	UPROPERTY(EditAnywhere, Category = "Colors")
	UMaterialInstanceConstant * SideColor6;

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void UndoRotation();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void RedoRotation();

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	UCommandHistory* GetHistory();

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	void SetHistory(UCommandHistory* NewHistory);

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	void OnHistoryLoaded();

	UPROPERTY(BlueprintReadOnly)
	bool IsReady;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	float RotationSpeed;

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
	UCommandHistory* CommandHistory;

	Command::Ptr CurrentCommand;
	float CommandProgress;

	TSharedPtr<RC::CubeParts> Parts;

	bool IsNeedUpdateParts;

	bool IsMovingFront;

private:
	void InitCube();
	void InitCubePart(UWorld * const world, const Coord& coord);

	UMaterialInstanceConstant * GetSideMaterial(int sideNum);
	void AttachSidesToSockets(UWorld * const world, AActor * actor, const Coord& coord);

	void UpdateParts();

	void FinishRotation();

	friend class RC::CubeCommand;
	friend class RC::RotationCommand;

};
