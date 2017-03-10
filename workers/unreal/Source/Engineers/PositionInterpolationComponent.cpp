// Fill out your copyright notice in the Description page of Project Settings.

#include "Engineers.h"
#include "PositionInterpolationComponent.h"


UPositionInterpolationComponent::UPositionInterpolationComponent():
    UInterpolationComponentBase(),
	Interpolator(0.2f), TimeInterpolatorLastQueried(0)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPositionInterpolationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPositionInterpolationComponent::AddPositionInterpolationValue(float Time, FVector Position)
{
	if(CheckForTimestampCorrection(Time))
	{
		Interpolator.Reset(TimeOffset, Position);
	}
	else
	{
		Interpolator.AddValue(Time, Position);
	}
}

FVector UPositionInterpolationComponent::GetInterpolatedPosition()
{
	auto Position = Interpolator.GetInterpolatedValue(TimeElapsed - TimeInterpolatorLastQueried);
	TimeInterpolatorLastQueried = TimeElapsed;
	return Position;
}

bool UPositionInterpolationComponent::ValidOutput()
{
	return Interpolator.HasValidOutput();
}