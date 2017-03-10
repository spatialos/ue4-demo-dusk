// Fill out your copyright notice in the Description page of Project Settings.

#include "Engineers.h"
#include "FloatInterpolationComponent.h"


UFloatInterpolationComponent::UFloatInterpolationComponent():
    UInterpolationComponentBase(),
	Interpolator(0.2f), TimeInterpolatorLastQueried(0)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFloatInterpolationComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

void UFloatInterpolationComponent::AddFloatInterpolationValue(float Time, float Value)
{
	if(CheckForTimestampCorrection(Time))
	{
		Interpolator.Reset(TimeOffset, Value);
	}
	else
	{
		Interpolator.AddValue(Time, Value);
	}
}

float UFloatInterpolationComponent::GetInterpolatedFloatValue()
{
	auto Float = Interpolator.GetInterpolatedValue(TimeElapsed - TimeInterpolatorLastQueried);
	TimeInterpolatorLastQueried = TimeElapsed;
	return Float;
}

bool UFloatInterpolationComponent::ValidOutput()
{
	return Interpolator.HasValidOutput();
}
