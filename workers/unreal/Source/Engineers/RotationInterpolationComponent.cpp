// Fill out your copyright notice in the Description page of Project Settings.

#include "Engineers.h"
#include "RotationInterpolationComponent.h"

URotationInterpolationComponent::URotationInterpolationComponent() :
    UInterpolationComponentBase(),
	Interpolator(0.2f), TimeInterpolatorLastQueried(0)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URotationInterpolationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void URotationInterpolationComponent::AddRotationInterpolationValue(float Time, FRotator Rotation)
{
    CheckForTimestampCorrection(Time);
	Interpolator.AddValue(TimeElapsed, Rotation);
}

FRotator URotationInterpolationComponent::GetInterpolatedRotation()
{
	auto Rotation = Interpolator.GetInterpolatedValue(TimeElapsed - TimeInterpolatorLastQueried);
	TimeInterpolatorLastQueried = TimeElapsed;
	return Rotation;
}

bool URotationInterpolationComponent::ValidOutput()
{
	return Interpolator.HasValidOutput();
}
