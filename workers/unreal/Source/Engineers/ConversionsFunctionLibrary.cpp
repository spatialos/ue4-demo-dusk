// Fill out your copyright notice in the Description page of Project Settings.

#include "Engineers.h"
#include "ConversionsFunctionLibrary.h"

FRotator UConversionsFunctionLibrary::GetSpatialOsToUnrealCoordinateSpace()
{
	return FRotator{ 0.0f, -90.0f, -90.0f };
}

float UConversionsFunctionLibrary::GetSpatialOsToUnrealScale()
{
	return 100.0;
}

FVector UConversionsFunctionLibrary::UnrealCoordinatesToSpatialOsCoordinates(const FVector& unrealCoordinates)
{
	return GetSpatialOsToUnrealCoordinateSpace().GetInverse().RotateVector(unrealCoordinates) / GetSpatialOsToUnrealScale();
}

FVector UConversionsFunctionLibrary::SpatialOsCoordinatesToUnrealCoordinates(const FVector& spatialOsCoordinates)
{
	return GetSpatialOsToUnrealCoordinateSpace().RotateVector(spatialOsCoordinates) * GetSpatialOsToUnrealScale();
}

FQuat UConversionsFunctionLibrary::UnrealRotationToSpatialOsRotation(const FQuat& unrealRotation)
{
	return GetSpatialOsToUnrealCoordinateSpace().GetInverse().Quaternion() * unrealRotation;
}

FQuat UConversionsFunctionLibrary::SpatialOsRotationToUnrealRotation(const FQuat& spatialOsRotation)
{
	return GetSpatialOsToUnrealCoordinateSpace().Quaternion() * spatialOsRotation;
}

improbable::transform::Quaternion UConversionsFunctionLibrary::FQuatToImprobableQuaternion(const FQuat& quaternion)
{
	return improbable::transform::Quaternion{ quaternion.W, quaternion.X, quaternion.Y, quaternion.Z };
}

FQuat UConversionsFunctionLibrary::ImprobableQuaternionToFQuat(UQuaternion* quaternion)
{
	return FQuat{ quaternion->GetX(), quaternion-> GetY(), quaternion->GetZ(), quaternion->GetW() };
}

FRotator UConversionsFunctionLibrary::QuaternionToRotator(FQuat quaternion)
{
	return FRotator{ quaternion };
}

FQuat UConversionsFunctionLibrary::RotatorToQuaternion(FRotator rotator)
{
	return rotator.Quaternion();
}

improbable::math::Coordinates UConversionsFunctionLibrary::FVectorToImprobableMathCoordinates(const FVector& vector)
{
	auto v = UnrealCoordinatesToSpatialOsCoordinates(vector);
	return improbable::math::Coordinates(static_cast<double>(v.X), static_cast<double>(v.Y), static_cast<double>(v.Z));
}

improbable::math::Vector3d UConversionsFunctionLibrary::FVectorToImprobableMathVector3d(const FVector& vector)
{
	auto v = UnrealCoordinatesToSpatialOsCoordinates(vector);
	return improbable::math::Vector3d(static_cast<double>(v.X), static_cast<double>(v.Y), static_cast<double>(v.Z));
}
