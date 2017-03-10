// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"
#include "TetherComponent.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class ENGINEERS_API UTetherComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Tether, BlueprintReadWrite)
	UClass* ConstraintClass;

	UPROPERTY(Category = Tether, BlueprintReadWrite)
	bool Tethered;

	UPROPERTY(Category = Tether, BlueprintReadWrite)
	AActor* Constraint;

	virtual void OnRegister() override;
};
