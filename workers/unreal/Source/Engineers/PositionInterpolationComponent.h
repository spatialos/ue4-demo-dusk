// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InterpolationComponentBase.h"
#include "TransformInterpolator.h"
#include "Components/ActorComponent.h"
#include "PositionInterpolationComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ENGINEERS_API UPositionInterpolationComponent : public UInterpolationComponentBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPositionInterpolationComponent();

	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UFUNCTION(BlueprintCallable, Category = "PositionInterpolation")
	void AddPositionInterpolationValue(float Time, FVector Position);

	UFUNCTION(BlueprintPure, Category = "PositionInterpolation")
	FVector GetInterpolatedPosition();

	UFUNCTION(BlueprintPure, Category = "PositionInterpolation")
	bool ValidOutput();

private:
	FVectorInterpolator Interpolator;

	float TimeInterpolatorLastQueried;
};
