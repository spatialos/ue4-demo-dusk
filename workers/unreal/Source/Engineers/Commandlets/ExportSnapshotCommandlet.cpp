// Fill out your copyright notice in the Description page of Project Settings.

#include "Engineers.h"
#include "ExportSnapshotCommandlet.h"

#include "ConversionsFunctionLibrary.h"
#include "improbable/collections.h"
#include "improbable/math/coordinates.h"
#include "improbable/math/vector3d.h"
#include "improbable/math/vector3f.h"
#include <improbable/worker.h>
#include "improbable/standard_library.h"
#include "improbable/transform/transformInfo.h"
#include "improbable/character/character_controller.h"
#include "improbable/character/character_movement_info.h"
#include "improbable/health/health.h"
#include "improbable/level/day_night_cycle.h"
#include "improbable/animation/animationinfo.h"
#include "improbable/forcefield/forcefieldinfo.h"
#include "../SpatialEntities/EntityFactory.h"

using namespace improbable;
using namespace improbable::math;
using namespace improbable::transform;
using namespace improbable::character;
using namespace improbable::level;
using namespace improbable::health;
using namespace improbable::animation;
using namespace improbable::spawner;
using namespace improbable::forcefield;

namespace {
	// Snapshot entity ID start values
	const int SPAWNER_ID = 200;
	const int MANTIS_SPAWNER_ID = 300;
	const int DAY_NIGHT_CYCLE_ID = 600;
	const int PIG_ID = 700;
	const int MANTIS_ID_START = 800;
	const int ROCK_ID_START = 900;
	const int GENERATOR_ID_START = 1000;
	const int HQBASE_ID_START = 1100;
	const int BATTERY_ID_START = 1200;

	// quantity defines
	const int SPHERICAL_NR_MANTIS_ENTITIES_TO_STORE = 3;

	const TArray<FVector> MANTIS_START_SPAWNPOINTS = {
		{-21868.0f, -10473.0f, 33600.0f}
		,{-19187.0f, -4148.0f, 32955.0f}
		,{ -20220.0f, 4343.0f, 34454.0f}
		,{ -8368.0f, 4343.0f, 34454.0f }
		,{ 2168.0f, 4343.0f, 31730.0f }
		,{ 11932.0f, 7281.0f, 34166.0f }
		,{ 9137.0f, -9519.0f, 32258.0f }
		,{ 154.0f, -30167.0f, 33885.0f }
		,{ -6747.0f, -32372.0f, 33712.0f }
		,{ -16101.0f, -32927.0f, 35637.0f }
	};
	const int NR_MANTIS_PER_SPAWNPOINT = 1;

	const int NR_ROCKS_TO_SPAWN = 30;
	const float ROCK_SPAWN_AREA_RADIUS = 10000.0;

	const TArray<std::string> ROCK_ASSET_NAMES = {
		"RawResource_Irridium",
		"RawResource_Metal"
	};

	improbable::transform::Quaternion IdentityQuaternion{ 1.0f, 0.0f, 0.0f, 0.0f };
}  // ::

// #define SPAWN_MANTIS_SPHERICAL 1

UExportSnapshotCommandlet::UExportSnapshotCommandlet()
{

}

UExportSnapshotCommandlet::~UExportSnapshotCommandlet()
{

}

int32 UExportSnapshotCommandlet::Main(const FString& Params)
{
	FString combinedPath = FPaths::Combine(*FPaths::GetPath(FPaths::GetProjectFilePath()), TEXT("../../snapshots"));

	TArray<FString> Args;
	Params.ParseIntoArrayWS(Args);

	// FVector Vec(FCString::Atof(*Args[1]), FCString::Atof(*Args[2]), FCString::Atof(*Args[3]));
	float startTimeInSeconds = FCString::Atof(*Args[1]);
	FString snapshotName = Args[2];
	
	UE_LOG(LogTemp, Display, TEXT("Combined path %s"), *combinedPath);

	if (FPaths::CollapseRelativeDirectories(combinedPath))
	{
		FString fullPath = FPaths::Combine(*combinedPath, snapshotName);

		std::unordered_map<worker::EntityId, worker::SnapshotEntity> entityMap;

		// -- Add Spawner
		entityMap.emplace(SPAWNER_ID, CreateSpawnerSnapshotEntity());
		entityMap.emplace(MANTIS_SPAWNER_ID, CreateMantisSpawnerSnapshotEntity());

		// -- Add Mantis NPCs
#ifdef SPAWN_MANTIS_SPHERICAL
		FVector sphericalBaseLocation(-3960, -13880, 31670);
		CreateMantisNPCEntitiesSpherical(entityMap, sphericalBaseLocation);
#else
		CreateMantisNPCSpawnPoints(entityMap);
#endif

		FVector RockSpawnBaseLocation(-3960, -13880, 31670);
		CreateRockSpawnPoints(entityMap, RockSpawnBaseLocation);

		// -- Add Day Night Cycle 
		UE_LOG(LogTemp, Display, TEXT("Adding DayAndNightCycle Entity to Snapshot..."));
		entityMap.emplace(DAY_NIGHT_CYCLE_ID, CreateDayNightCycleSnapshotEntity(startTimeInSeconds));

		UE_LOG(LogTemp, Display, TEXT("Adding PIG Entity to Snapshot..."));
		entityMap.emplace(PIG_ID, CreateHQObjectSnapshotEntity(FVector(-7794.8f, -10651.0f, 31643.7f), FRotator{-0.75f, 80.0f, -4.30f}.Quaternion(), "Blender", 1000.0f, 1000.0f));

		UE_LOG(LogTemp, Display, TEXT("Adding Generator Entities to Snapshot..."));
		entityMap.emplace(GENERATOR_ID_START, CreateGeneratorSnapshotEntity(FVector(-2893.553223f, -11379.129883f, 31593.0f), FVector{ 0.0f, 169.999908f, 0.0f}.Rotation().Quaternion(), "GeneratorBP", "Forcefield2", 0.0f, 100.0f));
		entityMap.emplace(GENERATOR_ID_START+1, CreateGeneratorSnapshotEntity(FVector(-7840.0f, -14060.0f, 31771.0f), FQuat::Identity, "GeneratorBP", "Forcefield3", 0.0f, 100.0f));
		entityMap.emplace(GENERATOR_ID_START+2, CreateGeneratorSnapshotEntity(FVector(-5620.0f, -17400.0f, 32180.0f), FQuat::Identity, "GeneratorBP", "Forcefield_236", 0.0f, 100.0f));

		UE_LOG(LogTemp, Display, TEXT("Adding HQ Entity to Snapshot..."));
		entityMap.emplace(HQBASE_ID_START, CreateHQObjectSnapshotEntity(FVector(-4172.493652f, -12631.717773f, 32086.000000f), FVector{ 0.0f, 104.375053f, 0.0f }.Rotation().Quaternion(), "Base_BP", 5000.0f, 5000.0f));

		UE_LOG(LogTemp, Display, TEXT("Adding a couple of  Battery Entities to Snapshot..."));
		entityMap.emplace(BATTERY_ID_START, CreateHQObjectSnapshotEntity(FVector(-5020.0f, -17400.0f, 33180.0), FQuat::Identity, "Battery_BP", 100.0f, 100.0f));
		entityMap.emplace(BATTERY_ID_START+1, CreateHQObjectSnapshotEntity(FVector(-5990.0f, -16400.0f, 33180.0), FQuat::Identity, "Battery_BP", 100.0f, 100.0f));

		entityMap.emplace(BATTERY_ID_START+2, CreateHQObjectSnapshotEntity(FVector(-7000.0f, -13510.0f, 32881.0f), FQuat::Identity, "Battery_BP", 100.0f, 100.0f));
		entityMap.emplace(BATTERY_ID_START+3, CreateHQObjectSnapshotEntity(FVector(-7700.0f, -11510.0f, 32881.0f), FQuat::Identity, "Battery_BP", 100.0f, 100.0f));

		entityMap.emplace(BATTERY_ID_START+4, CreateHQObjectSnapshotEntity(FVector(-2093.553223f, -11379.129883f, 32593.0f), FQuat::Identity, "Battery_BP", 100.0f, 100.0f));
		entityMap.emplace(BATTERY_ID_START+5, CreateHQObjectSnapshotEntity(FVector(-2993.553223f, -12379.129883f, 32593.0f), FQuat::Identity, "Battery_BP", 100.0f, 100.0f));

		// -- Write the snapshot to disc
		UE_LOG(LogTemp, Display, TEXT("Trying to write Snapshot to the path %s with %d entries"), *fullPath, entityMap.size());
		auto result = worker::SaveSnapshot(TCHAR_TO_UTF8(*fullPath), entityMap);
		if (result.empty())
		{
			UE_LOG(LogTemp, Display, TEXT("Snapshot exported to the path %s"), *fullPath);
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("ERROR writing Snapshot. Error message: %s"), *result->data());
		}

/*		worker::SaveSnapshot(TCHAR_TO_UTF8(*fullPath), { 
//			{454, CreateBlueprintTestSnapshotEntity(Vec)},
			//{500, CreateNPCSnapshotEntity() },
			{600, CreateDayNightCycleSnapshotEntity() }
		});
*/

		//worker::SaveSnapshot(TCHAR_TO_UTF8(*fullPath), { { 500, CreateNPCSnapshotEntity() } });
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("bye world!"));
	}

	return 0;
}

void UExportSnapshotCommandlet::CreateMantisNPCEntitiesSpherical(std::unordered_map<worker::EntityId, worker::SnapshotEntity>& EntityMap, const FVector& RootSpawnLocation) const
{
	UE_LOG(LogTemp, Display, TEXT("Adding %d Mantis NPCs spherically to Snapshot..."), SPHERICAL_NR_MANTIS_ENTITIES_TO_STORE);
	 
	float SpawnCircleRadius = 10000;
	for (int i = 0; i < SPHERICAL_NR_MANTIS_ENTITIES_TO_STORE; ++i)
	{
		float angle = (360.0 * i) / ((float)SPHERICAL_NR_MANTIS_ENTITIES_TO_STORE);
		float spawnX = RootSpawnLocation.X + SpawnCircleRadius * FMath::Cos(angle);
		float spawnY = RootSpawnLocation.Y + SpawnCircleRadius * FMath::Sin(angle);
		FVector spawnLocation = FVector(spawnX, spawnY, RootSpawnLocation.Z + 50000);
		EntityMap.emplace(MANTIS_ID_START + i, CreateMantisNPCSnapshotEntity(spawnLocation));
	}
}

void UExportSnapshotCommandlet::CreateMantisNPCSpawnPoints(std::unordered_map<worker::EntityId, worker::SnapshotEntity>& EntityMap) const
{
	UE_LOG(LogTemp, Display, TEXT("Adding %d Mantis NPCs from spawnpoints position to Snapshot..."), NR_MANTIS_PER_SPAWNPOINT * MANTIS_START_SPAWNPOINTS.Num());

	int idIndex = 0;
	for (int i = 0; i < MANTIS_START_SPAWNPOINTS.Num(); ++i)
	{
		for (int x = 0; x < NR_MANTIS_PER_SPAWNPOINT; ++x)
		{
			FVector randomPos = MANTIS_START_SPAWNPOINTS[i] + (FMath::VRand() * 1000.0f);
			randomPos.Z = MANTIS_START_SPAWNPOINTS[i].Z;
			EntityMap.emplace(MANTIS_ID_START + idIndex, CreateMantisNPCSnapshotEntity(randomPos));
			++idIndex;
		}
	}
}

void UExportSnapshotCommandlet::CreateRockSpawnPoints(std::unordered_map<worker::EntityId, worker::SnapshotEntity>& EntityMap, FVector SpawnOrigin) const
{
	UE_LOG(LogTemp, Display, TEXT("Adding %d rocks to snapshot..."), NR_ROCKS_TO_SPAWN);

	int idIndex = 0;
	for (int i = 0; i < NR_ROCKS_TO_SPAWN; ++i)
	{
		FVector randomPos;
		randomPos.X = FMath::FRandRange(SpawnOrigin.X - ROCK_SPAWN_AREA_RADIUS, SpawnOrigin.X + ROCK_SPAWN_AREA_RADIUS);
		randomPos.Y = FMath::FRandRange(SpawnOrigin.Y - ROCK_SPAWN_AREA_RADIUS, SpawnOrigin.Y + ROCK_SPAWN_AREA_RADIUS);
		randomPos.Z = 34000;

		std::string RockType = ROCK_ASSET_NAMES[FMath::RandHelper(ROCK_ASSET_NAMES.Num() - 1)];
		EntityMap.emplace(ROCK_ID_START + i, CreateAsteroidSnapshotEntity(randomPos, RockType));
	}
}

worker::SnapshotEntity UExportSnapshotCommandlet::CreateBlueprintTestSnapshotEntity(const FVector& SpawnLoc) const
{
	auto snapshotEntity = worker::SnapshotEntity();

	snapshotEntity.Prefab = "BIGSUIT_character_class";

	snapshotEntity.Add<TransformInfo>(
		TransformInfo::Data{ UConversionsFunctionLibrary::FVectorToImprobableMathCoordinates(SpawnLoc),
			IdentityQuaternion, 0.0f });

	improbable::WorkerRequirementSet serverRequirementSet({{{{{"UnrealWorker"}}}}});
	improbable::WorkerRequirementSet clientRequirementSet({{{{{"UnrealClient"}}}}});

	// -- All ACLs

	// Handle all authority delegations below.
	worker::Map<std::uint32_t, improbable::WorkerRequirementSet> componentAuthority;
	
	// Assign authority of TransformInfo component to UnrealClient
	componentAuthority.emplace(TransformInfo::ComponentId, clientRequirementSet);
		
	improbable::ComponentAcl componentAcl(componentAuthority);

	// -- All Requirements
		
	auto serverAttributeList = worker::List<improbable::WorkerAttribute>({worker::Option<std::string>("UnrealWorker")});
	auto clientAttributeList = worker::List<improbable::WorkerAttribute>({worker::Option<std::string>("UnrealClient")});

	// Combine the server and client attribute lists into a set.
	// Pas sthe set as the combined requirements for all workers.
	auto combinedAttributeSets = worker::List<improbable::WorkerAttributeSet>({{serverAttributeList}, {clientAttributeList}});
	improbable::WorkerRequirementSet combinedRequirementSet(combinedAttributeSets);
		
	// -- Set ACLs and Requirements on the Entity 
	snapshotEntity.Add<EntityAcl>(EntityAcl::Data(combinedRequirementSet, componentAcl));
	return snapshotEntity;
}

worker::SnapshotEntity UExportSnapshotCommandlet::CreateDayNightCycleSnapshotEntity(float startTimeInSeconds) const
{
	auto snapshotEntity = worker::SnapshotEntity();

	// Prefab / Blueprint
	snapshotEntity.Prefab = "DayNightCycleEntity";

	// Components
	snapshotEntity.Add<TransformInfo>(TransformInfo::Data{ UConversionsFunctionLibrary::FVectorToImprobableMathCoordinates(FVector::ZeroVector), IdentityQuaternion, 0.0f });
	
	// (((Time_In_Seconds / 3600) mod 0.666) / 0.666) * 24
	snapshotEntity.Add<DayNightCycle>(DayNightCycle::Data(startTimeInSeconds));

	// Read Authority
	auto read_access = UEntityFactory::UnrealClientOrUnrealWorker();

	// Write Authority
	worker::Map<std::uint32_t, improbable::WorkerRequirementSet> write_access;
	write_access.emplace(TransformInfo::ComponentId, UEntityFactory::UnrealClientOnly());
	write_access.emplace(DayNightCycle::ComponentId, UEntityFactory::UnrealWorkerOnly());

	// -- Set ACLs and Requirements on the Entity 
	snapshotEntity.Add<EntityAcl>(EntityAcl::Data(
		read_access,
		improbable::ComponentAcl(write_access)
	));

	return snapshotEntity;
}


worker::SnapshotEntity UExportSnapshotCommandlet::CreateNPCSnapshotEntity() const
{
	auto snapshotEntity = worker::SnapshotEntity();

	snapshotEntity.Prefab = "BIGSUIT_NPC";

	snapshotEntity.Add<TransformInfo>(
		TransformInfo::Data{ UConversionsFunctionLibrary::FVectorToImprobableMathCoordinates(FVector(-5000, -15000, 35000)),
			IdentityQuaternion, 0.0f});

	snapshotEntity.Add<CharacterMovementInfo>(
		CharacterMovementInfo::Data{ UConversionsFunctionLibrary::FVectorToImprobableMathVector3d(FVector()), 0.0f , 0.0f });

	improbable::WorkerRequirementSet serverRequirementSet({ { { { { "UnrealWorker" } } } } });
	improbable::WorkerRequirementSet clientRequirementSet({ { { { { "UnrealClient" } } } } });

	// -- All ACLs

	// Handle all authority delegations below.
	worker::Map<std::uint32_t, improbable::WorkerRequirementSet> componentAuthority;

	// Assign authority of TransformInfo compone to UnrealClient
	componentAuthority.emplace(TransformInfo::ComponentId, serverRequirementSet);
	componentAuthority.emplace(CharacterMovementInfo::ComponentId, serverRequirementSet);

	improbable::ComponentAcl componentAcl(componentAuthority);

	// -- All Requirements

	auto serverAttributeList = worker::List<improbable::WorkerAttribute>({ worker::Option<std::string>("UnrealWorker") });
	auto clientAttributeList = worker::List<improbable::WorkerAttribute>({ worker::Option<std::string>("UnrealClient") });

	// Combine the server and client attribute lists into a set.
	// Pas sthe set as the combined requirements for all workers.
	auto combinedAttributeSets = worker::List<improbable::WorkerAttributeSet>({ { serverAttributeList },{ clientAttributeList } });
	improbable::WorkerRequirementSet combinedRequirementSet(combinedAttributeSets);

	// -- Set ACLs and Requirements on the Entity 
	snapshotEntity.Add<EntityAcl>(EntityAcl::Data(combinedRequirementSet, componentAcl));
	return snapshotEntity;
}

worker::SnapshotEntity UExportSnapshotCommandlet::CreateMantisNPCSnapshotEntity(const FVector& SpawnLocation) const
{
	auto snapshotEntity = worker::SnapshotEntity();

	// Prefab / Blueprint
	snapshotEntity.Prefab = "Mantis_v2";

	// Components
	snapshotEntity.Add<TransformInfo>(TransformInfo::Data{ UConversionsFunctionLibrary::FVectorToImprobableMathCoordinates(SpawnLocation), improbable::transform::Quaternion{1.0f, 0.0f, 0.0f, 0.0f}, 0.0f });
	snapshotEntity.Add<CharacterMovementInfo>(CharacterMovementInfo::Data{ UConversionsFunctionLibrary::FVectorToImprobableMathVector3d(FVector()), 0.0f, 0.0f});
	snapshotEntity.Add<HealthInfo>(HealthInfo::Data(100.0f, 100.0f));
	snapshotEntity.Add<AnimationInfo>(AnimationInfo::Data{ "", 0.0f});

	// Read Authority
	auto read_access = UEntityFactory::UnrealClientOrUnrealWorker();

	// Write Authority
	worker::Map<std::uint32_t, improbable::WorkerRequirementSet> write_access;
	write_access.emplace(TransformInfo::ComponentId, UEntityFactory::UnrealWorkerOnly());
	write_access.emplace(CharacterMovementInfo::ComponentId, UEntityFactory::UnrealWorkerOnly());
	write_access.emplace(HealthInfo::ComponentId, UEntityFactory::UnrealWorkerOnly());
	write_access.emplace(AnimationInfo::ComponentId, UEntityFactory::UnrealWorkerOnly());

	// -- Set ACLs and Requirements on the Entity 
	snapshotEntity.Add<EntityAcl>(EntityAcl::Data(read_access, improbable::ComponentAcl(write_access)));

	return snapshotEntity;
}

worker::SnapshotEntity UExportSnapshotCommandlet::CreateSpawnerSnapshotEntity() const
{
	auto snapshotEntity = worker::SnapshotEntity();
	snapshotEntity.Prefab = "Spawner";

	snapshotEntity.Add<TransformInfo>(TransformInfo::Data{ UConversionsFunctionLibrary::FVectorToImprobableMathCoordinates(FVector::ZeroVector), improbable::transform::Quaternion{1.0f, 0.0f, 0.0f, 0.0f}, 0.0f });
	snapshotEntity.Add<spawner::Spawner>(spawner::Spawner::Data{});

	// Read Authority
	auto read_access = UEntityFactory::UnrealClientOrUnrealWorker();

	// Write Authority
	worker::Map<std::uint32_t, improbable::WorkerRequirementSet> write_access;
	write_access.emplace(TransformInfo::ComponentId, UEntityFactory::UnrealWorkerOnly());
	write_access.emplace(Spawner::ComponentId, UEntityFactory::UnrealWorkerOnly());

	// -- Set ACLs and Requirements on the Entity 
	snapshotEntity.Add<EntityAcl>(EntityAcl::Data(read_access, improbable::ComponentAcl(write_access)));

	return snapshotEntity;
}

worker::SnapshotEntity UExportSnapshotCommandlet::CreateAsteroidSnapshotEntity(const FVector& SpawnLocation, std::string RockAssetName) const
{
	auto snapshotEntity = worker::SnapshotEntity();

	// Prefab / Blueprint
	snapshotEntity.Prefab = RockAssetName;

	// Components
	snapshotEntity.Add<TransformInfo>(TransformInfo::Data{ UConversionsFunctionLibrary::FVectorToImprobableMathCoordinates(SpawnLocation), improbable::transform::Quaternion{1.0f, 0.0f, 0.0f, 0.0f}, 0.0f });

	// Read Authority
	auto read_access = UEntityFactory::UnrealClientOrUnrealWorker();

	// Write Authority
	worker::Map<std::uint32_t, improbable::WorkerRequirementSet> write_access;
	write_access.emplace(TransformInfo::ComponentId, UEntityFactory::UnrealWorkerOnly());
	
	// -- Set ACLs and Requirements on the Entity 
	snapshotEntity.Add<EntityAcl>(EntityAcl::Data(read_access, improbable::ComponentAcl(write_access)));
	return snapshotEntity;
}

worker::SnapshotEntity UExportSnapshotCommandlet::CreateHQObjectSnapshotEntity(const FVector& SpawnLocation, const FQuat& SpawnRotation, const std::string& PrefabName, float InitialHealth, float MaxHealth) const
{
	auto snapshotEntity = worker::SnapshotEntity();

	// Prefab / Blueprint
	snapshotEntity.Prefab = PrefabName;

	// Components
	snapshotEntity.Add<TransformInfo>(TransformInfo::Data(UConversionsFunctionLibrary::FVectorToImprobableMathCoordinates(SpawnLocation), UConversionsFunctionLibrary::FQuatToImprobableQuaternion(SpawnRotation), 0.0f));
	snapshotEntity.Add<HealthInfo>(HealthInfo::Data(InitialHealth, MaxHealth));

	// Read Authority
	auto read_access = UEntityFactory::UnrealClientOrUnrealWorker();

	// Write Authority
	worker::Map<std::uint32_t, improbable::WorkerRequirementSet> write_access;
	write_access.emplace(TransformInfo::ComponentId, UEntityFactory::UnrealWorkerOnly());
	write_access.emplace(HealthInfo::ComponentId, UEntityFactory::UnrealWorkerOnly());

	// -- Set ACLs and Requirements on the Entity 
	snapshotEntity.Add<EntityAcl>(EntityAcl::Data(read_access, improbable::ComponentAcl(write_access)));

	return snapshotEntity;
}

worker::SnapshotEntity UExportSnapshotCommandlet::CreateGeneratorSnapshotEntity(const FVector& SpawnLocation, const FQuat& SpawnRotation, const std::string& PrefabName, const std::string& ForcefieldName, float InitialHealth, float MaxHealth) const
{
	auto snapshotEntity = worker::SnapshotEntity();

	// Prefab / Blueprint
	snapshotEntity.Prefab = PrefabName;

	// Components
	snapshotEntity.Add<TransformInfo>(TransformInfo::Data(UConversionsFunctionLibrary::FVectorToImprobableMathCoordinates(SpawnLocation), UConversionsFunctionLibrary::FQuatToImprobableQuaternion(SpawnRotation), 0.0f));
	snapshotEntity.Add<HealthInfo>(HealthInfo::Data(InitialHealth, MaxHealth));
	snapshotEntity.Add<ForcefieldInfo>(ForcefieldInfo::Data(ForcefieldName));

	// Read Authority
	auto read_access = UEntityFactory::UnrealClientOrUnrealWorker();

	// Write Authority
	worker::Map<std::uint32_t, improbable::WorkerRequirementSet> write_access;
	write_access.emplace(TransformInfo::ComponentId, UEntityFactory::UnrealWorkerOnly());
	write_access.emplace(HealthInfo::ComponentId, UEntityFactory::UnrealWorkerOnly());
	write_access.emplace(ForcefieldInfo::ComponentId, UEntityFactory::UnrealWorkerOnly());

	// -- Set ACLs and Requirements on the Entity 
	snapshotEntity.Add<EntityAcl>(EntityAcl::Data(read_access, improbable::ComponentAcl(write_access)));

	return snapshotEntity;
}

worker::SnapshotEntity UExportSnapshotCommandlet::CreateMantisSpawnerSnapshotEntity() const
{
	auto snapshotEntity = worker::SnapshotEntity();
	snapshotEntity.Prefab = "Mantis_Spawner";

	snapshotEntity.Add<TransformInfo>(TransformInfo::Data{ UConversionsFunctionLibrary::FVectorToImprobableMathCoordinates(FVector::ZeroVector), improbable::transform::Quaternion{1.0f, 0.0f, 0.0f, 0.0f}, 0.0f });

	// Read Authority
	auto read_access = UEntityFactory::UnrealClientOrUnrealWorker();

	// Write Authority
	worker::Map<std::uint32_t, improbable::WorkerRequirementSet> write_access;
	write_access.emplace(TransformInfo::ComponentId, UEntityFactory::UnrealWorkerOnly());

	// -- Set ACLs and Requirements on the Entity 
	snapshotEntity.Add<EntityAcl>(EntityAcl::Data(read_access, improbable::ComponentAcl(write_access)));

	return snapshotEntity;
}
