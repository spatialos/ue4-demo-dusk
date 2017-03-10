// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SpatialOSGameMode.h"
//#include "WorkerRequirementSet.h"
#include "EngineersGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ENGINEERS_API AEngineersGameMode : public ASpatialOSGameMode
{
	GENERATED_BODY()

public:

	AEngineersGameMode();
	virtual ~AEngineersGameMode();
	
	void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "EngineersGameMode")
		FString GetWorkerId();

	DECLARE_DYNAMIC_DELEGATE_ThreeParams(FGetSpawnerEntityIdResultDelegate, bool, success, FString, errorMessage, int, spawnerEntityId);
	UFUNCTION(BlueprintCallable, Category = "EngineersGameMode")
	void GetSpawnerEntityId(const FGetSpawnerEntityIdResultDelegate& callback, int timeoutMs);

	UFUNCTION(BlueprintCallable, Category = "WorldRadiationOLD")
	FRotator GetCoordFromPosition(UObject* obj, FVector pos);
	UFUNCTION(BlueprintPure, Category = "SpatialOS")
	AActor* GetActorFromEntityId(int EntityId);
	
protected:
	void StartPlay() override;
	void ForceGarbageCollectionOnServer();

private:
	static AEngineersGameMode* Instance;
	FGetSpawnerEntityIdResultDelegate* mGetSpawnerEntityIdResultCallback;
	FTimerHandle ForcedGCTimerHandle;
};
