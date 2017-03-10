// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "InterpolationComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ENGINEERS_API UInterpolationComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInterpolationComponentBase();

	virtual void BeginPlay() override;
	
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

protected:
    bool CheckForTimestampCorrection(float NewTimestamp);

	float TimeElapsed;
    float TimeOffset;

private:
    bool bFirstTimestampReceived;
};
