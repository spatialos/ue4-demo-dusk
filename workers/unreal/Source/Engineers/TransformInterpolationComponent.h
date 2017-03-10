// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InterpolationComponentBase.h"
#include "TransformInterpolator.h"
#include "Improbable/Generated/cpp/unreal/TransformInfoComponent.h"
#include "TransformInterpolationComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ENGINEERS_API UTransformInterpolationComponent : public UInterpolationComponentBase
{
	GENERATED_BODY()

public:
	UTransformInterpolationComponent();

	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "TransformInterpolation")
	void SetInterpolationDelay(float InterpolationDelaySeconds);

	UFUNCTION(BlueprintCallable, Category = "TransformInterpolation")
	void SetEasingFactor(float EasingFactor);

private:
	UFUNCTION()
	void OnComponentUpdate(UTransformInfoComponentUpdate* update);
	UFUNCTION()
	void OnAuthorityChange(bool newAuthority);
	UFUNCTION()
	void OnComponentReady();

	void SetActorTransform();

	TransformInterpolationData GetInterpolatedTransformValue();



	TransformInterpolator Interpolator;

	bool bAuthoritative;
	bool bIniitialPositionSet;

	float TimeInterpolatorLastQueried;
	UTransformInfoComponent* TransformInfoComponent;
	UPrimitiveComponent* PrimitiveComponent;
	UStaticMeshComponent* StaticMesh;
};
