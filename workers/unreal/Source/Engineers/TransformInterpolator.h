#pragma once
#include "DelayedLinearInterpolator.h"
#include "EngineMinimal.h"

struct TransformInterpolationData
{
	FVector Position;
	FQuat Rotation;
	FVector PivotOffset;

	FVector GlobalPivot() const
	{
		return (Rotation.RotateVector(PivotOffset)) + Position;
	}
};

class TransformInterpolator : public improbable::unreal::DelayedLinearInterpolator<TransformInterpolationData>
{
public:
	TransformInterpolator():
		improbable::unreal::DelayedLinearInterpolator<TransformInterpolationData>(0.2f)
	{
	}

	explicit TransformInterpolator(float TimeDelay):
		improbable::unreal::DelayedLinearInterpolator<TransformInterpolationData>(TimeDelay)
	{
	}

	TransformInterpolationData Interpolate(const TransformInterpolationData& FromValue, const TransformInterpolationData& ToValue, float ProgressRatio) override
	{
		auto Rotation = FQuat::Slerp(FromValue.Rotation, ToValue.Rotation, ProgressRatio);
		auto PivotOffset = FMath::Lerp(FromValue.PivotOffset, ToValue.PivotOffset, ProgressRatio);
		auto GlobalPivot = FMath::Lerp(FromValue.GlobalPivot(), ToValue.GlobalPivot(), ProgressRatio);
		auto Position = GlobalPivot - (Rotation * PivotOffset);
		return TransformInterpolationData{ Position, Rotation, PivotOffset };
	}
};

class FVectorInterpolator : public improbable::unreal::DelayedLinearInterpolator<FVector>
{
public:
	FVectorInterpolator():
		improbable::unreal::DelayedLinearInterpolator<FVector>(0.2f)
	{
	}

	explicit FVectorInterpolator(float TimeDelay):
		improbable::unreal::DelayedLinearInterpolator<FVector>(TimeDelay)
	{
	}

	FVector Interpolate(const FVector& FromValue, const FVector& ToValue, float ProgressRatio) override
	{
		return FromValue * (1 - ProgressRatio) + ToValue * ProgressRatio;
	}
};

class RotatorInterpolator : public improbable::unreal::DelayedLinearInterpolator<FRotator>
{
public:
	RotatorInterpolator():
		improbable::unreal::DelayedLinearInterpolator<FRotator>(0.2f)
	{
	}

	explicit RotatorInterpolator(float TimeDelay):
		improbable::unreal::DelayedLinearInterpolator<FRotator>(TimeDelay)
	{
	}

	FRotator Interpolate(const FRotator& FromValue, const FRotator& ToValue, float ProgressRatio) override
	{
		return FromValue * (1 - ProgressRatio) + ToValue * ProgressRatio;
	}
};

class FloatInterpolator : public improbable::unreal::DelayedLinearInterpolator<float>
{
public:
	FloatInterpolator():
		improbable::unreal::DelayedLinearInterpolator<float>(0.2f)
	{
	}

	explicit FloatInterpolator(float TimeDelay):
		improbable::unreal::DelayedLinearInterpolator<float>(TimeDelay)
	{
	}

	float Interpolate(const float& FromValue, const float& ToValue, float ProgressRatio) override
	{
		return FromValue * (1 - ProgressRatio) + ToValue * ProgressRatio;
	}
};

class AnimationStateInterpolator : public improbable::unreal::DelayedLinearInterpolator<FString>
{
public:
	AnimationStateInterpolator():
		improbable::unreal::DelayedLinearInterpolator<FString>(0.2f)
	{
	}

	explicit AnimationStateInterpolator(float TimeDelay):
		improbable::unreal::DelayedLinearInterpolator<FString>(TimeDelay)
	{
	}

	FString Interpolate(const FString& FromValue, const FString& ToValue, float ProgressRatio) override
	{
		if (ProgressRatio > 0.99) 
		{
			return ToValue;
		}
		else
		{
			return FromValue;
		}
	}
};