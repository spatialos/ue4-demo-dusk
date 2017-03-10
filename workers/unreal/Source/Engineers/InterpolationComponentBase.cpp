// Fill out your copyright notice in the Description page of Project Settings.

#include "Engineers.h"
#include "InterpolationComponentBase.h"


// Sets default values for this component's properties
UInterpolationComponentBase::UInterpolationComponentBase():
    TimeElapsed(0), TimeOffset(0), bFirstTimestampReceived(false)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInterpolationComponentBase::BeginPlay()
{
	Super::BeginPlay();
}

void UInterpolationComponentBase::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
    TimeElapsed += DeltaTime;
}

bool UInterpolationComponentBase::CheckForTimestampCorrection(float NewTimestamp)
{
    if (!bFirstTimestampReceived || NewTimestamp - TimeElapsed > 0.4f)
    {
		TimeElapsed = 0.0f;
        TimeOffset = NewTimestamp;
        bFirstTimestampReceived = true;
		return true;
    }
	return false;
}

