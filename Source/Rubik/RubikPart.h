// All right! Yeah

#pragma once

#include "GameFramework/Actor.h"
#include "RubikPart.generated.h"

struct Coord {
	Coord(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
	int x, y, z;
};

UCLASS()
class RUBIK_API ARubikPart : public AActor
{
	GENERATED_BODY()

public:

	Coord InitialPos = Coord(-1, -1, -1);
};
