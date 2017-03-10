// Fill out your copyright notice in the Description page of Project Settings.

#include "Engineers.h"
#include "StringInterpolationComponent.h"

UStringInterpolationComponent::UStringInterpolationComponent():
    UInterpolationComponentBase(),
	Interpolator(0.2f), TimeInterpolatorLastQueried(0)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStringInterpolationComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

void UStringInterpolationComponent::AddStringInterpolationValue(float Time, FString Value)
{
    CheckForTimestampCorrection(Time);
	Interpolator.AddValue(TimeElapsed, Value);
}

FString UStringInterpolationComponent::GetInterpolatedStringValue()
{
	auto String = Interpolator.GetInterpolatedValue(TimeElapsed - TimeInterpolatorLastQueried);
	TimeInterpolatorLastQueried = TimeElapsed;
	return String;
}

bool UStringInterpolationComponent::ValidOutput()
{
	return Interpolator.HasValidOutput();
}
