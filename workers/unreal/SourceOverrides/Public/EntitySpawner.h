// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
#include "EngineMinimal.h"
#include "ScopedViewCallbacks.h"
#include <improbable/worker.h>

#pragma once

namespace improbable
{
namespace unreal
{
namespace entity_spawning
{
/**
 * Optional class that can be used to instantiate UClasses that are associated
 * with a prefab.
 */
class FEntitySpawner
{
public:
  FEntitySpawner(worker::Connection* Connection, worker::View* View, UWorld* World);
  //~FEntitySpawner();

  /** Associate a prefab with a UClass to create. */
  void RegisterPrefabName(const FString& PrefabName, UClass* ClassToSpawn);

  /**
   * Get the SpatialOs EntityId associated with an Unreal AActor.
   * Returns -1 if no associated id found.
   */
  worker::EntityId GetEntityId(AActor* Actor) const;

  /**
   * Get Unreal AActor associated with a SpatialOs EntityId.
   * Returns nullptr if no associated actor found.
   */
  AActor* GetEntityActor(worker::EntityId EntityId) const;

private:
  void OnAddEntity(const worker::AddEntityOp& Op);
  void OnRemoveEntity(const worker::RemoveEntityOp& Op);

	worker::View* View;
	worker::Connection* Connection;
	UWorld* World;
	UPROPERTY()
	TMap<FString, UClass*> ClassMap;
	UPROPERTY()
	TMap<int64, AActor*> EntityIdToActor;
	UPROPERTY()
	TMap<AActor*, worker::EntityId> ActorToEntityId;
	callbacks::FScopedViewCallbacks Callbacks;
};
}  // ::entity_spawning
}  // ::unreal
}  // ::improbable
