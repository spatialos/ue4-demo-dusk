// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "TargetingComponent.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class ENGINEERS_API UTargetingComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

	//UPROPERTY(SimpleDisplay, Category = Targeting, BlueprintReadWrite)
	//float FreeEnergy;

	UPROPERTY(SimpleDisplay, Category = Targeting, BlueprintReadWrite)
	UCameraComponent* Camera;

	UPROPERTY(SimpleDisplay, Category = Targeting, BlueprintReadOnly)
	FHitResult Hit;


	UFUNCTION(BlueprintCallable, Category = Targeting)
	bool GetTargetPoint(FHitResult& theHit) const;

public:
	virtual void OnRegister() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
};
