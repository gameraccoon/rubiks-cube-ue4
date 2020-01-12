// All right! Yeah

#pragma once

#include "GameFramework/Actor.h"
#include "RubikPart.h"
#include "RCRotationAxis.h"
#include "RubiksSide_Standart.generated.h"

namespace RC
{
	struct MovementDirection
	{
		MovementDirection(const FVector& _direction, RC::RotationAxis _axis, int _layerIndex)
			: direction(_direction)
			, axis(_axis)
			, layerIndex(_layerIndex)
		{
		}

		FVector direction;
		RC::RotationAxis axis;
		int layerIndex;
	};
}

UCLASS()
class RUBIK_API ARubiksSide_Standart : public ARubikPart
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARubiksSide_Standart(const class FObjectInitializer& OI);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void SetDirections(const TArray<RC::MovementDirection>& directions);
	TArray<RC::MovementDirection> GetDirections() const;

	void SetColorIndex(int NewColorIndex) { ColorIndex = NewColorIndex; }
	int GetColorIndex() const { return ColorIndex; }

	void SetCurrentSideIndex(int NewSideIndex) { SideIndex = NewSideIndex; }
	int GetSideIndex() const { return SideIndex; }

	void SetInitialSideIndex(int InSideIndex) { InitialSideIndex = InSideIndex; }
	int GetInitialSideIndex() const { return InitialSideIndex; }

private:
	TArray<RC::MovementDirection> Directions;
	int ColorIndex = -1;
	int SideIndex = -1;
	int InitialSideIndex = -1;
};
