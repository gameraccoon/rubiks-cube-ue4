// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RubicsCube.generated.h"

// An abstract action that we can do and undo
class Command
{
public:
	typedef TSharedRef<Command> Ref;
	typedef TSharedPtr<Command> Ptr;

public:
	virtual ~Command() = 0;

	/// execute the command
	virtual void Execute() = 0;
	/// undo actions of the command
	virtual void Unexecute() = 0;
	/// is command continious in time
	virtual bool IsContinious() = 0;
	/// set progress (for continious commands)
	virtual void SetProgress(float progress) = 0;
};

UCLASS()
class RUBIK_API ARubicsCube : public AActor
{
	GENERATED_BODY()
public:

	/** size in elements */
	UPROPERTY(EditAnywhere, Category = "Parameters", meta = (ClampMin = "2", ClampMax = "5", UIMin = "2", UIMax = "5"))
	uint8 Count;

	/** size in units */
	UPROPERTY(EditAnywhere, Category = "Parameters")
	float Size;

	/** cubic's type */
	UPROPERTY(EditAnywhere, Category = "Parameters")
	FName Type;

public:	
	// Sets default values for this actor's properties
	ARubicsCube();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

private:
	struct CubePart
	{
		struct Coord {
			Coord(int x, int y, int z) : x(x), y(y), z(z) {}
			int x, y, z;
		};
	};

private:
	TArray<AActor*> cubeParts;
	TDoubleLinkedList<Command::Ref> commandsHistory;
	TDoubleLinkedList<Command::Ref>::TIterator commandsHead;

private:
	void InitCube();
	void InitCubePart(const CubePart::Coord& coord);

	friend class CubicCommand;
};
