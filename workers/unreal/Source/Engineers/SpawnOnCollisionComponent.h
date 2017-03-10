// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "UObject/PersistentObjectPtr.h"
#include "SpawnOnCollisionComponent.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class ENGINEERS_API USpawnOnCollisionComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(Category = "SpawnOnCollision", BlueprintReadWrite, EditAnywhere)
	TAssetSubclassOf<AActor> SpawnObject;

	UPROPERTY(Category = "SpawnOnCollision", BlueprintReadWrite, EditAnywhere)
	float SpawnTime;

	UPROPERTY(Category = "SpawnOnCollision", BlueprintReadWrite)
	FTransform SpawnTransform;

	UPROPERTY(Category = "SpawnOnCollision", BlueprintReadWrite)
	bool Collided;

	UPROPERTY(Category = "SpawnOnCollision", BlueprintReadWrite)
	float ExclusionRange;
};
