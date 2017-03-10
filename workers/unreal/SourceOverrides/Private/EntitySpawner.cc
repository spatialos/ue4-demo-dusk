// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
#include "EntitySpawner.h"
#include "Engine.h"
#include "EntityId.h"
#include "SpatialOsComponent.h"
#include <functional>

namespace improbable
{
namespace unreal
{
namespace entity_spawning
{
DECLARE_LOG_CATEGORY_EXTERN(LogFEntitySpawner, Log, All);
DEFINE_LOG_CATEGORY(LogFEntitySpawner);

FEntitySpawner::FEntitySpawner(worker::Connection* Connection, worker::View* View, UWorld* World)
: View(View), Connection(Connection), World(World), Callbacks(View)
{
  Callbacks.Add(
      View->OnAddEntity(std::bind(&FEntitySpawner::OnAddEntity, this, std::placeholders::_1)));
  Callbacks.Add(View->OnRemoveEntity(
      std::bind(&FEntitySpawner::OnRemoveEntity, this, std::placeholders::_1)));
}

void FEntitySpawner::RegisterPrefabName(const FString& PrefabName, UClass* ClassToSpawn)
{
  UClass** ExistingClass = ClassMap.Find(PrefabName);
  if (ExistingClass != nullptr)
  {
    UE_LOG(LogFEntitySpawner, Error, TEXT("PrefabName '%s' has already been registered to '%s'."),
           *PrefabName, *(*ExistingClass)->GetClass()->GetName());
    return;
  }

  ClassMap.Add(PrefabName, ClassToSpawn);
}

void FEntitySpawner::OnAddEntity(const worker::AddEntityOp& Op)
{
  FString PrefabName = UTF8_TO_TCHAR(Op.PrefabName.c_str());
  auto ClassToSpawn = ClassMap.Find(PrefabName);
  if (ClassToSpawn == nullptr && *ClassToSpawn != nullptr && !(*ClassToSpawn)->IsValidLowLevel() && (*ClassToSpawn)->GetName() != "invalid")
  {
    UE_LOG(LogFEntitySpawner, Warning, TEXT("Can't spawn EntityId(%s). No UClass is associated "
                                            "with PrefabName('%s') so no Actor will be spawned."),
           *ToString(Op.EntityId), *PrefabName);
    return;
  }

  auto Actor = World->SpawnActor(*ClassToSpawn);

  auto levelscriptActor = World->GetLevelScriptActor();
  if (levelscriptActor != nullptr)
  {
	  Actor->SetOwner(levelscriptActor);
  }

  EntityIdToActor.Add(Op.EntityId, Actor);
  ActorToEntityId.Add(Actor, Op.EntityId);

  auto SpatialOsComponents = Actor->GetComponentsByClass(USpatialOsComponent::StaticClass());
  worker::List<worker::ComponentId> ComponentIds;
  for (auto Component : SpatialOsComponents)
  {
    USpatialOsComponent* SpatialOsComponent = Cast<USpatialOsComponent>(Component);
    SpatialOsComponent->Init(Connection, View, Op.EntityId);
    ComponentIds.emplace_back(SpatialOsComponent->GetComponentId());
  }
  Connection->SendInterestedComponents(Op.EntityId, ComponentIds);
}

void FEntitySpawner::OnRemoveEntity(const worker::RemoveEntityOp& Op)
{
  auto Actor = EntityIdToActor.Find(Op.EntityId);
  if (Actor == nullptr || *Actor == nullptr)
  {
    UE_LOG(LogFEntitySpawner, Warning,
           TEXT("Can't remove EntityId(%s), no Actor is associated with it."),
           *ToString(Op.EntityId));
    return;
  }

  ActorToEntityId.Remove(*Actor);
  if (!(*Actor)->IsPendingKill() && (*Actor)->IsValidLowLevel())
  {
	  World->DestroyActor(*Actor);
  }

  EntityIdToActor.Remove(Op.EntityId);
}

worker::EntityId FEntitySpawner::GetEntityId(AActor* Actor) const
{
  auto EntityId = ActorToEntityId.Find(Actor);
  if (EntityId == nullptr)
  {
    UE_LOG(LogFEntitySpawner, Warning,
           TEXT("Can't find EntityId for Actor(%s), no EntityId is associated with it."),
           *Actor->GetName());
    return -1;
  }
  return *EntityId;
}

AActor* FEntitySpawner::GetEntityActor(worker::EntityId EntityId) const
{
  if (!EntityIdToActor.Contains(EntityId))
  {
    return nullptr;
  }

  return EntityIdToActor[EntityId];
}
}  // ::entity_spawning
}  // ::unreal
}  // ::improbable
