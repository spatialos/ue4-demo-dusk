// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "GameFramework/GameMode.h"

#include "EntitySpawner.h"
#include "Improbable/Generated/cpp/unreal/Commander.h"
#include "SpatialOSWorkerConfiguration.h"
#include "WorkerConnection.h"
#include "SpatialOSGameMode.generated.h"

UCLASS()
class
// clang-format off
ENGINEERS_API ASpatialOSGameMode : public AGameModeBase
// clang-format on
{
  GENERATED_BODY()

public:
  ASpatialOSGameMode();
  virtual ~ASpatialOSGameMode();

  improbable::unreal::entity_spawning::FEntitySpawner* GetSpawner();

  UFUNCTION(BlueprintPure, Category = "SpatialOS")
  int GetEntityId(AActor * actor);

  UFUNCTION(BlueprintCallable, Category = "SpatialOS")
  FString SpatialOsWorkerType();

  improbable::unreal::core::FWorkerConnection& WorkerConnection();

  UFUNCTION(BlueprintImplementableEvent, Category = "SpatialOS")
  void OnSpatialOsConnected();

  UFUNCTION(BlueprintImplementableEvent, Category = "SpatialOS")
  void OnSpatialOsDisconnected();

  UFUNCTION(BlueprintImplementableEvent, Category = "SpatialOS")
  void OnSpatialOsFailedToConnect();

  UFUNCTION(BlueprintPure, Category = "SpatialOS")
  static bool IsConnectedToSpatialOs();

  UPROPERTY(BluePrintReadWrite, EditDefaultsOnly, NoClear)
  FString WorkerType;

  UPROPERTY(BluePrintReadWrite, EditDefaultsOnly, NoClear)
  FString WorkerId;

  UFUNCTION(BlueprintPure, Category = "SpatialOS")
  UCommander* SendWorkerCommand();

  const improbable::unreal::SpatialOSWorkerConfiguration& GetWorkerConfiguration();

protected:
  void StartPlay() override;
  void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  void Tick(float DeltaTime) override;

  void RegisterBlueprintFolder(const FString& path);

private:
  void ConfigureWorker();
  improbable::unreal::SpatialOSWorkerConfiguration WorkerSettingsFromCmdLine() const;
  improbable::unreal::SpatialOSWorkerConfiguration WorkerSettingsUserOverride() const;

  void CreateWorkerConnection();
  void TeardownWorkerConnection();

  void RegisterEntityBlueprints();
  void RegisterEntityBlueprints(FString folder);

  improbable::unreal::SpatialOSWorkerConfiguration Worker;
  TUniquePtr<improbable::unreal::entity_spawning::FEntitySpawner> Spawner;

  std::vector<FString> BlueprintPaths;

  UPROPERTY()
  UCommander* Commander;
};
