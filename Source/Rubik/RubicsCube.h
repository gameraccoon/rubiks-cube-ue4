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
class RUBIK_API ARubicsCube : public AActor
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

public:
	// Sets default values for this actor's properties
	ARubicsCube(const class FObjectInitializer& OI);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void UpdateParts();

private:
	GameBase::CommandHistory commandHistory;

	TSharedPtr<RC::CubeParts> parts;

private:
	void InitCube();
	void InitCubePart(UWorld * const world, const RC::CubeParts::Coord& coord);

	UMaterialInstanceConstant * GetSideMaterial(const FVector& sidePos);
	void AttachSidesToSockets(UWorld * const world, AActor * actor, const RC::CubeParts::Coord& coord);

	friend class RC::CubeCommand;
	friend class RC::RotationCommand;

private:
	// test values
	float progress = 0.0f;
	int currentAxis = 0;
	int currentStep = 0;
	int steps = 0;
	bool front = true;
};
