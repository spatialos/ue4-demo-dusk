// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Commandlets/Commandlet.h"
#include <improbable/worker.h>
#include "ExportSnapshotCommandlet.generated.h"

/**
 * 
 */
UCLASS()
class ENGINEERS_API UExportSnapshotCommandlet : public UCommandlet
{
	GENERATED_BODY()
	
public:
	UExportSnapshotCommandlet();
	virtual ~UExportSnapshotCommandlet();

	virtual int32 Main(const FString& Params) override;

private:

	void CreateMantisNPCEntitiesSpherical(std::unordered_map<worker::EntityId, worker::SnapshotEntity>& EntityMap, const FVector& RootSpawnLocation) const;
	void CreateMantisNPCSpawnPoints(std::unordered_map<worker::EntityId, worker::SnapshotEntity>& EntityMap) const;
	void CreateRockSpawnPoints(std::unordered_map<worker::EntityId, worker::SnapshotEntity>& EntityMap, FVector SpawnOrigin) const;


	worker::SnapshotEntity CreateBlueprintTestSnapshotEntity(const FVector& SpawnLoc) const;
	worker::SnapshotEntity CreateNPCSnapshotEntity() const;
	worker::SnapshotEntity CreateDayNightCycleSnapshotEntity(float startTimeInSeconds) const;
	worker::SnapshotEntity CreateMantisNPCSnapshotEntity(const FVector& SpawnLocation) const;
	worker::SnapshotEntity CreateAsteroidSnapshotEntity(const FVector& SpawnLocation, std::string RockAssetName) const;
	worker::SnapshotEntity CreateSpawnerSnapshotEntity() const;
	worker::SnapshotEntity CreateGeneratorSnapshotEntity(const FVector& SpawnLocation, const FQuat& SpawnRotation, const std::string& PrefabName, const std::string& ForcefieldName, float InitialHealth, float MaxHealth) const;
	worker::SnapshotEntity CreateHQObjectSnapshotEntity(const FVector& SpawnLocation, const FQuat& SpawnRotation, const std::string& PrefabName, float InitialHealth, float MaxHealth) const;
	worker::SnapshotEntity CreateMantisSpawnerSnapshotEntity() const;
};
