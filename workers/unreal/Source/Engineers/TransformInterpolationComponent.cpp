// Fill out your copyright notice in the Description page of Project Settings.

#include "Engineers.h"
#include "TransformInterpolationComponent.h"
#include "SpatialOSGameMode.h"
#include "ConversionsFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

UTransformInterpolationComponent::UTransformInterpolationComponent() : UInterpolationComponentBase(),
                                                                       Interpolator(0.2f), TimeInterpolatorLastQueried(0), TransformInfoComponent(nullptr), PrimitiveComponent(nullptr), StaticMesh(nullptr)
{
}

void UTransformInterpolationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!TransformInfoComponent->HasBegunPlay())
	{
		return;
	}
	if(Interpolator.HasValidOutput() && !TransformInfoComponent->HasAuthority())
	{
		auto Transform = GetInterpolatedTransformValue();
		GetOwner()->SetActorLocationAndRotation(Transform.Position, Transform.Rotation); 
	}
	else if (TransformInfoComponent->HasAuthority())
	{
		improbable::transform::TransformInfo::Update rawUpdate;
		rawUpdate.set_position(UConversionsFunctionLibrary::FVectorToImprobableMathCoordinates(GetOwner()->GetActorLocation()))
			.set_rotation(UConversionsFunctionLibrary::FQuatToImprobableQuaternion(GetOwner()->GetActorQuat()))
			.set_timestamp(GetWorld()->GetTimeSeconds());
		auto* update = NewObject<UTransformInfoComponentUpdate>()->Init(rawUpdate);
		TransformInfoComponent->SendComponentUpdate(update);
	}
}

void UTransformInterpolationComponent::BeginPlay()
{
	Super::BeginPlay();
	TransformInfoComponent = GetOwner()->FindComponentByClass<UTransformInfoComponent>();
	PrimitiveComponent = GetOwner()->FindComponentByClass<UPrimitiveComponent>();
	StaticMesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
	TransformInfoComponent->OnAuthorityChange.AddDynamic(this, &UTransformInterpolationComponent::OnAuthorityChange);
	TransformInfoComponent->OnComponentReady.AddDynamic(this, &UTransformInterpolationComponent::OnComponentReady);
	TransformInfoComponent->OnComponentUpdate.AddDynamic(this, &UTransformInterpolationComponent::OnComponentUpdate);
}

void UTransformInterpolationComponent::SetInterpolationDelay(float InterpolationDelaySeconds)
{
	Interpolator.SetInterpolationDelay(InterpolationDelaySeconds);
}

void UTransformInterpolationComponent::SetEasingFactor(float EasingFactor)
{
	Interpolator.SetEasingFactor(EasingFactor);
}

void UTransformInterpolationComponent::OnAuthorityChange(bool newAuthority) 
{
	SetActorTransform();
	StaticMesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
	if (StaticMesh != nullptr)
	{
		StaticMesh->SetSimulatePhysics(newAuthority);
	}
}

void UTransformInterpolationComponent::OnComponentReady()
{
	SetActorTransform();
}

void UTransformInterpolationComponent::OnComponentUpdate(UTransformInfoComponentUpdate*)
{
	
	if(TransformInfoComponent->HasAuthority())
	{
		return;
	}
	auto Position = UConversionsFunctionLibrary::SpatialOsCoordinatesToUnrealCoordinates(TransformInfoComponent->GetPosition());
	auto Rotation = UConversionsFunctionLibrary::ImprobableQuaternionToFQuat(TransformInfoComponent->GetRotation());
	auto PivotOffset = PrimitiveComponent->GetCenterOfMass();

	if(CheckForTimestampCorrection(TransformInfoComponent->GetTimestamp()))
	{
		Interpolator.Reset(TimeOffset, Position, Rotation, PivotOffset);
	}
	else
	{
		Interpolator.AddValue(TransformInfoComponent->GetTimestamp(), Position, Rotation, PivotOffset);
	}
}

void UTransformInterpolationComponent::SetActorTransform()
{
	GetOwner()->SetActorLocationAndRotation(UConversionsFunctionLibrary::SpatialOsCoordinatesToUnrealCoordinates(TransformInfoComponent->GetPosition()), 
		UConversionsFunctionLibrary::ImprobableQuaternionToFQuat(TransformInfoComponent->GetRotation()).Rotator()); 
}

TransformInterpolationData UTransformInterpolationComponent::GetInterpolatedTransformValue()
{
	auto interpolatedValue = Interpolator.GetInterpolatedValue(TimeElapsed - TimeInterpolatorLastQueried);
	TimeInterpolatorLastQueried = TimeElapsed;
	return interpolatedValue;
}