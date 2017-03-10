// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InterpolationComponentBase.h"
#include "TransformInterpolator.h"
#include "Components/ActorComponent.h"
#include "RotationInterpolationComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ENGINEERS_API URotationInterpolationComponent : public UInterpolationComponentBase
{
	GENERATED_BODY()

public:	
	URotationInterpolationComponent();

	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UFUNCTION(BlueprintCallable, Category = "RotationInterpolation")
	void AddRotationInterpolationValue(float Time, FRotator Rotation);

	UFUNCTION(BlueprintPure, Category = "RotationInterpolation")
	FRotator GetInterpolatedRotation();

	UFUNCTION(BlueprintPure, Category = "RotationInterpolation")
	bool ValidOutput();

private:
	RotatorInterpolator Interpolator;

	float TimeInterpolatorLastQueried;
};
