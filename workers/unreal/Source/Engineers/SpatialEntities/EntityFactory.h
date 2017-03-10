// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <improbable/worker.h>
#include "improbable/standard_library.h"
#include "Improbable/Generated/cpp/unreal/EntityTemplate.h"
#include "EntityFactory.generated.h"

UCLASS()
class ENGINEERS_API UEntityFactory : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "EntityFactory")
	static UEntityTemplate* CreatePlayerEntity(FString workerId, const FVector& SpawnLoc);

	UFUNCTION(BlueprintCallable, Category = "EntityFactory")
	static UEntityTemplate* CreateBatteryEntity(const FVector& SpawnLoc);

	UFUNCTION(BlueprintCallable, Category = "EntityFactory")
	static UEntityTemplate* CreateMantisEntity(const FVector& SpawnLoc);

	UFUNCTION(BlueprintCallable, Category = "EntityFactory")
	static UEntityTemplate* CreateAsteroidEntity(const FVector& SpawnLoc);

	UFUNCTION(BlueprintCallable, Category = "EntityFactory")
	static UEntityTemplate* CreateRockEntity(const FVector& SpawnLoc);

	// Requirement sets
	static improbable::WorkerRequirementSet UnrealWorkerOnly();
	static improbable::WorkerRequirementSet UnrealClientOnly();
	static improbable::WorkerRequirementSet UnrealClientOrUnrealWorker();
	static improbable::WorkerRequirementSet SpecificWorker(FString workerId);
};
