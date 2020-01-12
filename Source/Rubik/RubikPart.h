// All right! Yeah

#pragma once

#include "GameFramework/Actor.h"
#include "RubikPart.generated.h"

struct Coord
{
	Coord() : x(-1), y(-1), z(-1) {}
	Coord(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
	int x, y, z;
};

UCLASS()
class RUBIK_API ARubikPart : public AActor
{
	GENERATED_BODY()

public:

	Coord InitialPos;
};
