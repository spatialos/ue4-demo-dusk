// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
// clang-format off
#include "Engineers.h"
// clang-format on
#include "SpatialOSGameMode.h"
#include "CommandLineArgumentReader.h"
#include "Engine.h"

#define ENTITY_BLUEPRINTS_FOLDER "/Game/EntityBlueprints"

using improbable::unreal::core::FWorkerConnection;

namespace
{
FString GenerateRandomWorkerIdFromTypeName(FString type)
{
  return type + FGuid::NewGuid().ToString();
}
}  // ::


ASpatialOSGameMode::ASpatialOSGameMode() : WorkerType(""), WorkerId("")
{
  PrimaryActorTick.bCanEverTick = true;
  RegisterBlueprintFolder(TEXT(ENTITY_BLUEPRINTS_FOLDER));
}

improbable::unreal::entity_spawning::FEntitySpawner* ASpatialOSGameMode::GetSpawner()
{
  return Spawner.Get();
}

ASpatialOSGameMode::~ASpatialOSGameMode()
{
}

FString ASpatialOSGameMode::SpatialOsWorkerType()
{
  return Worker.WorkerType;
}

FWorkerConnection& ASpatialOSGameMode::WorkerConnection()
{
  return FWorkerConnection::GetInstance();
}

void ASpatialOSGameMode::ConfigureWorker()
{
  Worker = GetWorld()->WorldType != EWorldType::Game ? WorkerSettingsUserOverride()
                                                     : WorkerSettingsFromCmdLine();
}

const improbable::unreal::SpatialOSWorkerConfiguration& ASpatialOSGameMode::GetWorkerConfiguration()
{
  return Worker;
}

improbable::unreal::SpatialOSWorkerConfiguration
ASpatialOSGameMode::WorkerSettingsFromCmdLine() const
{
  const FString CmdLineWorkerType = improbable::unreal::core::command_line::ParseWorkerType();

  FString CmdLineWorkerId = improbable::unreal::core::command_line::ParseWorkerId();
  if (CmdLineWorkerId.IsEmpty())
  {
    CmdLineWorkerId = GenerateRandomWorkerIdFromTypeName(CmdLineWorkerType);
  }

  return improbable::unreal::SpatialOSWorkerConfiguration(CmdLineWorkerType, CmdLineWorkerId);
}

improbable::unreal::SpatialOSWorkerConfiguration
ASpatialOSGameMode::WorkerSettingsUserOverride() const
{
  // in editor: allow worker type and id to be overriden by the user
  const FString OverrideWorkerType =
      WorkerType == FString{""} ? FString{"UnrealClient"} : WorkerType;

  const FString OverrideWorkerId =
      WorkerId.IsEmpty() ? GenerateRandomWorkerIdFromTypeName(OverrideWorkerType) : WorkerId;

  return improbable::unreal::SpatialOSWorkerConfiguration(OverrideWorkerType, OverrideWorkerId);
}

bool ASpatialOSGameMode::IsConnectedToSpatialOs()
{
  return FWorkerConnection::IsConnected();
}

void ASpatialOSGameMode::StartPlay()
{
  AGameModeBase::StartPlay();
  CreateWorkerConnection();
}

void ASpatialOSGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  AGameModeBase::EndPlay(EndPlayReason);
  TeardownWorkerConnection();
}

void ASpatialOSGameMode::Tick(float DeltaTime)
{
  AGameModeBase::Tick(DeltaTime);
  if (IsConnectedToSpatialOs())
  {
    FWorkerConnection::GetInstance().ProcessEvents();
  }
}

void ASpatialOSGameMode::CreateWorkerConnection()
{
  ConfigureWorker();

  // Parse commandline properties
  const FString receptionistIp = improbable::unreal::core::command_line::ParseReceptionistIp();
  const std::int32_t port =
      FCString::Atoi(*improbable::unreal::core::command_line::ParseReceptionistPort());
  const FString parsedLinkProtocol = improbable::unreal::core::command_line::ParseLinkProtocol();

  const auto linkProtocol = parsedLinkProtocol == "Tcp" ? worker::NetworkConnectionType::kTcp
                                                        : worker::NetworkConnectionType::kRaknet;

  const auto DeploymentName = improbable::unreal::core::command_line::ParseDeploymentName();
  const auto LoginToken = improbable::unreal::core::command_line::ParseLoginToken();
  const auto ProjectName = improbable::unreal::core::command_line::ParseProjectName();
  const bool ShouldConnectViaLocator = !LoginToken.IsEmpty();

  // Log parsed input
  UE_LOG(LogSpatialOS, Warning, TEXT(": receptionistIp %s, port %d, WorkerType %s, WorkerId %s"),
         *receptionistIp, port, *Worker.WorkerType, *Worker.WorkerId)

  // Setup connection
  FWorkerConnection::SetComponentMetaclasses(worker::GetComponentMetaclasses());
  FWorkerConnection::SetInstance(TUniquePtr<FWorkerConnection>(new FWorkerConnection()));
  FWorkerConnection::GetInstance().GetView().OnDisconnect(
      [this](const worker::DisconnectOp& disconnect) { OnSpatialOsDisconnected(); });

  worker::ConnectionParameters Params;
  Params.Network.ConnectionType = linkProtocol;
  Params.Network.UseExternalIp = ShouldConnectViaLocator;
  Params.WorkerType = TCHAR_TO_UTF8(*Worker.WorkerType);
  Params.WorkerId = TCHAR_TO_UTF8(*Worker.WorkerId);

  improbable::unreal::core::FOnConnectedDelegate OnConnected;
  OnConnected.BindLambda([this](bool HasConnected) {
    if (HasConnected)
    {
      UE_LOG(LogSpatialOS, Display, TEXT("Connected to SpatialOS"))
      this->Spawner.Reset(new improbable::unreal::entity_spawning::FEntitySpawner(
          &FWorkerConnection::GetInstance().GetConnection(),
          &FWorkerConnection::GetInstance().GetView(), GetWorld()));
      RegisterEntityBlueprints();
      OnSpatialOsConnected();
    }
    else
    {
      UE_LOG(LogSpatialOS, Error, TEXT("Failed to connect to SpatialOS"))
      OnSpatialOsFailedToConnect();
    }
  });

  improbable::unreal::core::FQueueStatusDelegate OnQueueStatus;
  OnQueueStatus.BindLambda([this](const worker::QueueStatus& Status) {
    if (Status.Error)
    {
      UE_LOG(LogSpatialOS, Error, TEXT("Error connecting to deployment: %s"),
             UTF8_TO_TCHAR(Status.Error->c_str()))
      return false;
    }
    else
    {
      UE_LOG(LogSpatialOS, Display, TEXT("Position in queue: %u"), Status.PositionInQueue)
      return true;
    }
  });

  if (ShouldConnectViaLocator)
  {
    FWorkerConnection::GetInstance().ConnectToLocatorAsync(
        ProjectName, DeploymentName, LoginToken, Params, OnQueueStatus, GetWorld(), OnConnected);
  }
  else
  {
    FWorkerConnection::GetInstance().ConnectToReceptionistAsync(receptionistIp, port, Params,
                                                                GetWorld(), OnConnected);
  }
}

void ASpatialOSGameMode::TeardownWorkerConnection()
{
  if (!FWorkerConnection::IsConnected())
  {
    return;
  }

  FWorkerConnection::GetInstance().Disconnect();
}

void ASpatialOSGameMode::RegisterBlueprintFolder(const FString& path)
{
  BlueprintPaths.push_back(path);
}

void ASpatialOSGameMode::RegisterEntityBlueprints()
{
  for (auto& path : BlueprintPaths)
  {
    RegisterEntityBlueprints(path);
  }
}

void ASpatialOSGameMode::RegisterEntityBlueprints(FString folder)
{
  TArray<UObject*> assets;
  if (EngineUtils::FindOrLoadAssetsByPath(folder, assets, EngineUtils::ATL_Class))
  {
    for (auto asset : assets)
    {
      UBlueprintGeneratedClass* blueprintGeneratedClass = Cast<UBlueprintGeneratedClass>(asset);
      if (blueprintGeneratedClass != nullptr)
      {
        FString blueprintName = blueprintGeneratedClass->GetName().LeftChop(
            2);  // generated blueprint class names end with "_C"
        UE_LOG(LogTemp, Warning, TEXT("Registering blueprint in entity spawner with name: %s"),
               *blueprintName)
        Spawner->RegisterPrefabName(blueprintName, blueprintGeneratedClass);
      }
      else
      {
        UE_LOG(LogTemp, Warning,
               TEXT("Found asset in the EntityBlueprints folder which is not a blueprint: %s"),
               *(asset->GetFullName()))
      }
    }
  }
  else
  {
    UE_LOG(LogTemp, Warning, TEXT("No assets found in EntityBlueprints folder."))
  }
}

UCommander* ASpatialOSGameMode::SendWorkerCommand()
{
  if (Commander == nullptr)
  {
    auto& WorkerConnection = FWorkerConnection::GetInstance();
    Commander = NewObject<UCommander>(this, UCommander::StaticClass())
                    ->Init(nullptr, &WorkerConnection.GetConnection(), &WorkerConnection.GetView());
  }
  return Commander;
}

int ASpatialOSGameMode::GetEntityId(AActor* actor)
{
  return static_cast<int>(Spawner->GetEntityId(actor));
}