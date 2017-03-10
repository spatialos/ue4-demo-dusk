// Fill out your copyright notice in the Description page of Project Settings.

#include "Engineers.h"
#include "EntityFactory.h"

#include "ConversionsFunctionLibrary.h"
#include "improbable/collections.h"
#include "improbable/math/coordinates.h"
#include "improbable/math/vector3d.h"
#include "improbable/math/vector3f.h"
#include <improbable/worker.h>
#include "improbable/standard_library.h"
#include "improbable/transform/transformInfo.h"
#include "improbable/character/character_movement_info.h"
#include "improbable/character/character_controller.h"
#include "improbable/health/health.h"
#include "improbable/aiming/player_aim_info.h"
#include "improbable/gravguntarget/grav_gun_target.h"

using namespace improbable;
using namespace improbable::math;
using namespace improbable::transform;
using namespace improbable::character;
using namespace improbable::aiming;
using namespace improbable::player;
using namespace improbable::health;
using namespace improbable::animation;

UEntityTemplate* UEntityFactory::CreatePlayerEntity(FString workerId, const FVector& SpawnLoc)
{
	auto entity = worker::Entity();

	// Read Authority
	auto read_access = UnrealClientOrUnrealWorker();

	// Write Authority
	worker::Map<std::uint32_t, improbable::WorkerRequirementSet> write_access;

	// Components
	entity.Add<TransformInfo>(TransformInfo::Data{
		UConversionsFunctionLibrary::FVectorToImprobableMathCoordinates(SpawnLoc),
		improbable::transform::Quaternion{1.0f, 0.0f, 0.0, 0.0f}, 0.0f }
	);

	entity.Add<HealthInfo>(HealthInfo::Data(1000.0f, 1000.0f));
	entity.Add<CharacterController>(CharacterController::Data());
	entity.Add<CharacterMovementInfo>(CharacterMovementInfo::Data{ UConversionsFunctionLibrary::FVectorToImprobableMathVector3d(FVector()), 0.0f, 0.0f});
	entity.Add<PlayerAimInfo>(PlayerAimInfo::Data(0.0f, false));
	entity.Add<GravGunTarget>(GravGunTarget::Data(0));
	entity.Add<HeartbeatSender>(HeartbeatSender::Data{});
	entity.Add<HeartbeatReceiver>(HeartbeatReceiver::Data{});


	// Component writers
	write_access.emplace(TransformInfo::ComponentId, SpecificWorker(workerId));
	write_access.emplace(HealthInfo::ComponentId, UnrealWorkerOnly());
	write_access.emplace(CharacterController::ComponentId, SpecificWorker(workerId));
	write_access.emplace(CharacterMovementInfo::ComponentId, SpecificWorker(workerId));
	write_access.emplace(PlayerAimInfo::ComponentId, SpecificWorker(workerId));
	write_access.emplace(GravGunTarget::ComponentId, SpecificWorker(workerId));
	write_access.emplace(HeartbeatReceiver::ComponentId, SpecificWorker(workerId));
	write_access.emplace(HeartbeatSender::ComponentId, UnrealWorkerOnly());

	// -- Set ACLs and Requirements on the Entity 
	entity.Add<EntityAcl>(EntityAcl::Data(
		read_access,
		improbable::ComponentAcl(write_access)
	));

	auto entityWrapper = NewObject<UEntityTemplate>(UEntityTemplate::StaticClass());
	entityWrapper->Init(entity);
	return entityWrapper;
}

UEntityTemplate* UEntityFactory::CreateBatteryEntity(const FVector& SpawnLoc)
{
	auto entity = worker::Entity();

	// Read Authority
	auto read_access = UnrealClientOrUnrealWorker();

	// Write Authority
	worker::Map<std::uint32_t, improbable::WorkerRequirementSet> write_access;

	// Components
	entity.Add<TransformInfo>(TransformInfo::Data{
		UConversionsFunctionLibrary::FVectorToImprobableMathCoordinates(SpawnLoc),
		improbable::transform::Quaternion{1.0f, 0.0f, 0.0f, 0.0f}, 0.0f }
	);

	entity.Add<HealthInfo>(HealthInfo::Data(100.0f, 100.0f));

	// Component writers
	write_access.emplace(TransformInfo::ComponentId, UnrealWorkerOnly());
	write_access.emplace(HealthInfo::ComponentId, UnrealWorkerOnly());

	// -- Set ACLs and Requirements on the Entity 
	entity.Add<EntityAcl>(EntityAcl::Data(read_access, improbable::ComponentAcl(write_access)));

	auto entityWrapper = NewObject<UEntityTemplate>(UEntityTemplate::StaticClass());
	entityWrapper->Init(entity);

	return entityWrapper;
}


UEntityTemplate* UEntityFactory::CreateMantisEntity(const FVector& SpawnLoc)
{
	auto entity = worker::Entity();

	// Read Authority
	auto read_access = UnrealClientOrUnrealWorker();

	// Write Authority
	worker::Map<std::uint32_t, improbable::WorkerRequirementSet> write_access;

	// Components
	entity.Add<TransformInfo>(TransformInfo::Data{ UConversionsFunctionLibrary::FVectorToImprobableMathCoordinates(SpawnLoc), improbable::transform::Quaternion{1.0f, 0.0f, 0.0f, 0.0f}, 0.0f });
	entity.Add<CharacterMovementInfo>(CharacterMovementInfo::Data{ UConversionsFunctionLibrary::FVectorToImprobableMathVector3d(FVector()), 0.0f, 0.0f });
	entity.Add<HealthInfo>(HealthInfo::Data(100.0f, 100.0f));
	entity.Add<AnimationInfo>(AnimationInfo::Data{ "", 0.0f });

	// Component writers
	write_access.emplace(TransformInfo::ComponentId, UnrealWorkerOnly());
	write_access.emplace(CharacterMovementInfo::ComponentId, UnrealWorkerOnly());
	write_access.emplace(HealthInfo::ComponentId, UnrealWorkerOnly());
	write_access.emplace(AnimationInfo::ComponentId, UnrealWorkerOnly());

	// -- Set ACLs and Requirements on the Entity 
	entity.Add<EntityAcl>(EntityAcl::Data(read_access, improbable::ComponentAcl(write_access)));

	auto entityWrapper = NewObject<UEntityTemplate>(UEntityTemplate::StaticClass());
	entityWrapper->Init(entity);

	return entityWrapper;
}


UEntityTemplate* UEntityFactory::CreateAsteroidEntity(const FVector& SpawnLoc)
{
	auto entity = worker::Entity();

	// Read Authority
	auto read_access = UnrealClientOrUnrealWorker();

	// Write Authority
	worker::Map<std::uint32_t, improbable::WorkerRequirementSet> write_access;

	// Components
	entity.Add<TransformInfo>(TransformInfo::Data{ UConversionsFunctionLibrary::FVectorToImprobableMathCoordinates(SpawnLoc), improbable::transform::Quaternion{1.0f, 0.0f, 0.0f, 0.0f}, 0.0f });

	// Component writers
	write_access.emplace(TransformInfo::ComponentId, UnrealWorkerOnly());

	// -- Set ACLs and Requirements on the Entity 
	entity.Add<EntityAcl>(EntityAcl::Data(read_access, improbable::ComponentAcl(write_access)));

	auto entityWrapper = NewObject<UEntityTemplate>(UEntityTemplate::StaticClass());
	entityWrapper->Init(entity);

	return entityWrapper;
}


UEntityTemplate* UEntityFactory::CreateRockEntity(const FVector& SpawnLoc)
{
	auto entity = worker::Entity();

	// Read Authority
	auto read_access = UnrealClientOrUnrealWorker();

	// Write Authority
	worker::Map<std::uint32_t, improbable::WorkerRequirementSet> write_access;

	// Components
	entity.Add<TransformInfo>(TransformInfo::Data{ UConversionsFunctionLibrary::FVectorToImprobableMathCoordinates(SpawnLoc), improbable::transform::Quaternion{1.0f, 0.0f, 0.0f, 0.0f}, 0.0f });

	// Component writers
	write_access.emplace(TransformInfo::ComponentId, UnrealWorkerOnly());

	// -- Set ACLs and Requirements on the Entity 
	entity.Add<EntityAcl>(EntityAcl::Data(read_access, improbable::ComponentAcl(write_access)));

	auto entityWrapper = NewObject<UEntityTemplate>(UEntityTemplate::StaticClass());
	entityWrapper->Init(entity);

	return entityWrapper;
}

// -- 

improbable::WorkerRequirementSet UEntityFactory::UnrealWorkerOnly()
{
	auto workerAttributes = worker::List<improbable::WorkerAttribute>({ worker::Option<std::string>("UnrealWorker") });
	auto attributeSet = worker::List<improbable::WorkerAttributeSet>({ { workerAttributes } });

	return improbable::WorkerRequirementSet(attributeSet);
}

improbable::WorkerRequirementSet UEntityFactory::UnrealClientOnly()
{
	auto workerAttributes = worker::List<improbable::WorkerAttribute>({ worker::Option<std::string>("UnrealClient") });
	auto attributeSet = worker::List<improbable::WorkerAttributeSet>({ { workerAttributes } });

	return improbable::WorkerRequirementSet(attributeSet);
}

improbable::WorkerRequirementSet UEntityFactory::UnrealClientOrUnrealWorker()
{
	auto serverAttributes = worker::List<improbable::WorkerAttribute>({ worker::Option<std::string>("UnrealWorker") });
	auto clientAttributes = worker::List<improbable::WorkerAttribute>({ worker::Option<std::string>("UnrealClient") });
	auto attributeSet = worker::List<improbable::WorkerAttributeSet>({ { serverAttributes },{ clientAttributes } });
	return improbable::WorkerRequirementSet(attributeSet);
}

improbable::WorkerRequirementSet UEntityFactory::SpecificWorker(FString workerId)
{
	std::string specificWorkerId = "workerId:";
	specificWorkerId += TCHAR_TO_UTF8(*workerId);

	auto workerAttributes = worker::List<improbable::WorkerAttribute>({ worker::Option<std::string>(specificWorkerId) });
	auto attributeSet = worker::List<improbable::WorkerAttributeSet>({ { workerAttributes } });

	return improbable::WorkerRequirementSet(attributeSet);
}

