// Fill out your copyright notice in the Description page of Project Settings.

#include "Rubik.h"
#include "RubicsCube.h"

// some abstract action with Rubik's Cube
class RubiksCubeCommand
	: public Command
{
public:
	RubiksCubeCommand(ARubicsCube * target)
		: target(target)
	{}

protected:
	/** get target for modification */
	ARubicsCube * GetTarget() { return target; }

private:
	// pointer to object that we will modify
	ARubicsCube * const target;
};

class RotationCommand
	: public RubiksCubeCommand
{
	enum Axis {
		FX, RX,
		FY, RY,
		FZ, RZ
	};

public:
	static Ref Create(ARubicsCube * target, Axis axis, int layerIdx)
	{
		return Ref(new RotationCommand(target, axis, layerIdx));
	}

	~RotationCommand()
	{

	}

	virtual void Execute() override
	{

	}

	virtual void Unexecute() override
	{

	}

	virtual void SetProgress(float progress) override
	{

	}

private:
	RotationCommand(ARubicsCube * target, Axis axis, int layerIdx)
		: RubiksCubeCommand(target)
		, axis(axis)
		, layerIndex(layerIdx)
	{}

private:
	// axis we rotating around (directed)
	const Axis axis;
	// index of layer that we rotating
	const int layerIndex;
};

// Sets default values
ARubicsCube::ARubicsCube()
	: Count(3)
	, Size(100)
	, Type("Standart")
	, commandsHead(commandsHistory.GetTail())
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARubicsCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARubicsCube::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ARubicsCube::InitCube()
{
	for (int k = 0; k < Count; ++k)
	{
		for (int j = 0; j < Count; ++j)
		{
			for (int i = 0; i < Count; ++i)
			{
				InitCubePart(CubePart::Coord(i, j, k));
			}
		}
	}
}

void ARubicsCube::InitCubePart(const CubePart::Coord& coord)
{

}