// Fill out your copyright notice in the Description page of Project Settings.

#include "Rubik.h"
#include "RubicsCube.h"


// Sets default values
ARubicsCube::ARubicsCube()
	: Count(3)
	, Size(100)
	, Type("Standart")
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