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

	UPROPERTY(EditAnywhere, Category = "CubeCompoents")
		UStaticMesh * Block1Board;

	UPROPERTY(EditAnywhere, Category = "CubeCompoents")
		UStaticMesh * Block2Board;

	UPROPERTY(EditAnywhere, Category = "CubeCompoents")
		UStaticMesh * Block3Board;

public:	
	// Sets default values for this actor's properties
	ARubicsCube(const class FObjectInitializer& OI);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

private:
	GameBase::CommandHistory commandHistory;

	RC::CubeParts parts;

	FVector centerShift;

private:
	void InitCube(const class FObjectInitializer& OI);
	void InitCubePart(const class FObjectInitializer& OI, const RC::CubeParts::Coord& coord);
	UStaticMesh * Init1BoardPart();
	UStaticMesh * Init2BoardPart();
	UStaticMesh * Init3BoardPart();
	UStaticMeshComponent * ConstructBlock(UStaticMesh * staticMesh, const class FObjectInitializer& OI, FVector location, FRotator rotation);
	FRotator InitBlockRotation(const RC::CubeParts::Coord& coord);

	friend class RC::CubeCommand;
	friend class RC::RotationCommand;
};
