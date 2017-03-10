// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InterpolationComponentBase.h"
#include "TransformInterpolator.h"
#include "Components/ActorComponent.h"
#include "StringInterpolationComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ENGINEERS_API UStringInterpolationComponent : public UInterpolationComponentBase
{
	GENERATED_BODY()

public:
	UStringInterpolationComponent();

	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UFUNCTION(BlueprintCallable, Category = "StringInterpolation")
	void AddStringInterpolationValue(float Time, FString Value);

	UFUNCTION(BlueprintPure, Category = "StringInterpolation")
	FString GetInterpolatedStringValue();

	UFUNCTION(BlueprintPure, Category = "StringInterpolation")
	bool ValidOutput();

private:
	AnimationStateInterpolator Interpolator;

	float TimeInterpolatorLastQueried;
};
