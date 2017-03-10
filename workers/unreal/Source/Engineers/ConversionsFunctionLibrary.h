// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include <improbable/math/coordinates.h>
#include <improbable/math/vector3d.h>
#include <improbable/math/vector3f.h>
#include <Improbable/Generated/cpp/unreal/Quaternion.h>
#include "ConversionsFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ENGINEERS_API UConversionsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category="Conversions")
    static FRotator GetSpatialOsToUnrealCoordinateSpace();

    UFUNCTION(BlueprintPure, Category="Conversions")
    static float GetSpatialOsToUnrealScale();

    UFUNCTION(BlueprintPure, Category="Conversions")
    static FVector UnrealCoordinatesToSpatialOsCoordinates(const FVector& unrealCoordinates);

    UFUNCTION(BlueprintPure, Category="Conversions")
    static FVector SpatialOsCoordinatesToUnrealCoordinates(const FVector& spatialOsCoordinates);

    UFUNCTION(BlueprintPure, Category="Conversions")
    static FQuat UnrealRotationToSpatialOsRotation(const FQuat& unrealRotation);

    UFUNCTION(BlueprintPure, Category="Conversions")
    static FQuat SpatialOsRotationToUnrealRotation(const FQuat& spatialOsRotation);

    UFUNCTION(BlueprintPure, Category="Conversions")
    static FQuat ImprobableQuaternionToFQuat(UQuaternion* quaternion);

    UFUNCTION(BlueprintPure, Category="Conversions")
    static FRotator QuaternionToRotator(FQuat quaternion);

    UFUNCTION(BlueprintPure, Category="Conversions")
    static FQuat RotatorToQuaternion(FRotator rotator);

    static improbable::transform::Quaternion FQuatToImprobableQuaternion(const FQuat& quaternion);
    static improbable::math::Coordinates FVectorToImprobableMathCoordinates(const FVector& vector);
    static improbable::math::Vector3d FVectorToImprobableMathVector3d(const FVector& vector);
	
};
