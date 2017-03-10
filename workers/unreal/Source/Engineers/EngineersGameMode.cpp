// Fill out your copyright notice in the Description page of Project Settings.

#include "Engineers.h"
#include "Engine.h"
#include "engPlayerControllerDefault.h"
#include "EngineersGameMode.h"
#include "Landscape.h"

#define ENTITY_BLUEPRINTS_FOLDER "/Game/EntityBlueprints"
#define CHARACTER_FOLDER "/Game/Character"
#define FAUNA_FOLDER "/Game/Fauna"
#define GENERAL_BLUEPRINTS_FOLDER "/Game/Blueprints"
#define BUILDINGS_BLUEPRINT_FOLDER "/Game/Buildings"

AEngineersGameMode* AEngineersGameMode::Instance;

AEngineersGameMode::AEngineersGameMode()
{
	// Set the default player controller class
	PlayerControllerClass = AengPlayerControllerDefault::StaticClass();

	// Don't spawn players automatically
	bStartPlayersAsSpectators = true;

	// No need for default pawn class
	DefaultPawnClass = nullptr;

	Instance = this;

	RegisterBlueprintFolder(CHARACTER_FOLDER);
	RegisterBlueprintFolder(FAUNA_FOLDER);
	RegisterBlueprintFolder(GENERAL_BLUEPRINTS_FOLDER);
	RegisterBlueprintFolder(BUILDINGS_BLUEPRINT_FOLDER);
}

AEngineersGameMode::~AEngineersGameMode()
{
	Instance = nullptr;
}

FString AEngineersGameMode::GetWorkerId()
{
	return GetWorkerConfiguration().WorkerId;
}

void AEngineersGameMode::Tick(float DeltaTime)
{
	ASpatialOSGameMode::Tick(DeltaTime);
}

void AEngineersGameMode::GetSpawnerEntityId(const FGetSpawnerEntityIdResultDelegate& callback, int timeoutMs)
{
	mGetSpawnerEntityIdResultCallback = new FGetSpawnerEntityIdResultDelegate(callback);
	const worker::query::EntityQuery& entity_query = {
		worker::query::ComponentConstraint { improbable::spawner::Spawner::ComponentId },
		worker::query::SnapshotResultType {}
	};

	auto requestId = WorkerConnection().GetConnection().SendEntityQueryRequest(entity_query, static_cast<std::uint32_t>(timeoutMs));
	WorkerConnection().GetView().OnEntityQueryResponse([this, requestId](const worker::EntityQueryResponseOp& op)
	{
		if (op.RequestId != requestId)
		{
			return;
		}
		if (!mGetSpawnerEntityIdResultCallback->IsBound())
		{
			UE_LOG(LogTemp, Warning,
				TEXT("mGetSpawnerEntityIdResultCallback is unbound"))
		}
		if (op.StatusCode != worker::StatusCode::kSuccess)
		{
			std::string errorMessage = "Could not find spawner entity: " + op.Message;
			mGetSpawnerEntityIdResultCallback->ExecuteIfBound(false, FString(errorMessage.c_str()), -1);
			return;
		}
		if (op.ResultCount == 0)
		{
			std::string errorMessage = "Query returned 0 spawner entities";
			mGetSpawnerEntityIdResultCallback->ExecuteIfBound(false, FString(errorMessage.c_str()), -1);
			return;
		}
		mGetSpawnerEntityIdResultCallback->ExecuteIfBound(true, FString(), static_cast<int>(op.Result.begin()->first));
		return;
	});
}

// Old WorldRadiation code thats still used.
FRotator AEngineersGameMode::GetCoordFromPosition(UObject* obj, FVector pos)
{
	if (GEngine == nullptr || obj == nullptr)
		return FRotator(0.0f);

	UWorld* world = GEngine->GetWorldFromContextObject(obj);
	if (world != NULL)
	{
		TActorIterator<ALandscape> itLandsc(world); // not as efficient as a pointer, but not bad (log(n))

		AActor* landsc = *itLandsc;
		if (landsc != nullptr)
		{
			FVector orig;
			FVector extnt;
			landsc->GetActorBounds(false, orig, extnt);

			FVector boundsMin = orig - extnt;

			FVector vinterp = (pos - boundsMin) / (2 * extnt);

			FVector coordA(-145.0f, -60.0f, 0.0f);
			FVector coordB(-35.0f, 60.0f, 0.0f);

			FVector coord = FMath::Lerp(coordA, coordB, vinterp);

			return FRotator(coord.X, 0.0f, coord.Y);
		}
	}
	return FRotator(0.0f);
}

AActor* AEngineersGameMode::GetActorFromEntityId(int EntityId)
{
	return GetSpawner()->GetEntityActor((worker::EntityId)EntityId);
}

void AEngineersGameMode::StartPlay()
{
	ASpatialOSGameMode::StartPlay();

	//if (SpatialOsWorkerType() == FString(TEXT("UnrealWorker")))
	//{
	//	GetWorldTimerManager().SetTimer(ForcedGCTimerHandle, this, &AEngineersGameMode::ForceGarbageCollectionOnServer, 60.0f, true, 15.0f);
	//}
}

void AEngineersGameMode::ForceGarbageCollectionOnServer()
{
	UWorld* world = GetWorld();
	if (world != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Forcing Garbage Collection on Server!"))
		world->ForceGarbageCollection(false);
	}
}
