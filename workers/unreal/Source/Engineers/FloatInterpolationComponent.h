// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InterpolationComponentBase.h"
#include "TransformInterpolator.h"
#include "Components/ActorComponent.h"
#include "FloatInterpolationComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ENGINEERS_API UFloatInterpolationComponent : public UInterpolationComponentBase
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFloatInterpolationComponent();

	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UFUNCTION(BlueprintCallable, Category = "FloatInterpolation")
	void AddFloatInterpolationValue(float Time, float Float);

	UFUNCTION(BlueprintPure, Category = "FloatInterpolation")
	float GetInterpolatedFloatValue();

	UFUNCTION(BlueprintPure, Category = "FloatInterpolation")
	bool ValidOutput();

private:
	FloatInterpolator Interpolator;

	float TimeInterpolatorLastQueried;
};