#pragma once
#include <cmath>
#include "DelayedLinearInterpolator.h"

namespace improbable
{
namespace unreal
{
template <typename TValue>
DelayedLinearInterpolator<TValue>::DelayedLinearInterpolator():
	PreviousValue(nullptr), CurrentValue(nullptr), InterpolationDelay(DEFAULT_INTERPOLATION_DELAY_SECONDS), EasingFactor(DEFAULT_EASING_FACTOR), bDelayChanged(false), CurrentTime(0.0f)
{
}

template<typename TValue>
DelayedLinearInterpolator<TValue>::DelayedLinearInterpolator(float InterpolationDelay):
	PreviousValue(nullptr), CurrentValue(nullptr), InterpolationDelay(InterpolationDelay), EasingFactor(DEFAULT_EASING_FACTOR), bDelayChanged(false), CurrentTime(0.0f)
{
}

template <typename TValue>
DelayedLinearInterpolator<TValue>::DelayedLinearInterpolator(float InterpolationDelay, float EasingFactor):
	PreviousValue(nullptr), CurrentValue(nullptr), InterpolationDelay(InterpolationDelay), EasingFactor(EasingFactor), bDelayChanged(false), CurrentTime(0.0f)
{
}

template <typename TValue>
DelayedLinearInterpolator<TValue>::~DelayedLinearInterpolator()
{
    ClearValue(CurrentValue);
    ClearValue(PreviousValue);
}

template <typename TValue>
template <typename... Args>
void DelayedLinearInterpolator<TValue>::Reset(float InitialValueAbsoluteTime, Args&&... args)
{
	while(!InterpolationValues.empty())
	{
		InterpolationValues.pop();
	}
	CurrentTime = InitialValueAbsoluteTime - InterpolationDelay;
	EnqueueNewValue(InitialValueAbsoluteTime, TValue{ std::forward<Args>(args)... });
	SetCurrentValue(InterpolationValues.front().second);
	SetPreviousValue(InterpolationValues.front().second);
}

template <typename TValue>
template <typename... Args>
void DelayedLinearInterpolator<TValue>::AddValue(float NewValueAbsoluteTime, Args&&... args)
{
	if(!HasPendingValues() || IsCurrentTimeAheadOf(NewValueAbsoluteTime) || bDelayChanged)
	{
		Reset(NewValueAbsoluteTime, std::forward<Args>(args)...);
	}
	else
	{
		EnqueueNewValue(NewValueAbsoluteTime, TValue{ std::forward<Args>(args)... });
	}
}

template <typename TValue>
bool DelayedLinearInterpolator<TValue>::HasValidOutput()
{
	return CurrentValue != nullptr;
}

template<typename TValue>
void DelayedLinearInterpolator<TValue>::SetInterpolationDelay(float InterpolationDelaySeconds)
{
	InterpolationDelay = InterpolationDelaySeconds;
	bDelayChanged = true;
}

template<typename TValue>
void DelayedLinearInterpolator<TValue>::SetEasingFactor(float InEasingFactor)
{
	EasingFactor = InEasingFactor;
}

template <typename TValue>
TValue DelayedLinearInterpolator<TValue>::GetInterpolatedValue(float DeltaTimeToAdvance)
{
	float PreviousTime = CurrentTime;
	CurrentTime += DeltaTimeToAdvance;
	DiscardOutdatedValues();
	SetCurrentValue(HasPendingValues() ? InterpolateToNextValue(PreviousTime) : *PreviousValue);
	return *CurrentValue;
}

template <typename TValue>
void DelayedLinearInterpolator<TValue>::DiscardOutdatedValues()
{
	while(!InterpolationValues.empty() && CurrentTime > InterpolationValues.front().first)
	{
		SetPreviousValue(InterpolationValues.front().second);
		InterpolationValues.pop();
	}
}

template <typename TValue>
TValue DelayedLinearInterpolator<TValue>::InterpolateToNextValue(float PreviousTime)
{
	const auto& PendingTimeStampedValue = InterpolationValues.front();
	ApplyTimeDriftCorrection(PendingTimeStampedValue.first);
	float ElapsedTime = CurrentTime - PreviousTime;
	float TimeUntilNextValue = PendingTimeStampedValue.first - PreviousTime;
	float TransitionRatio = ElapsedTime / TimeUntilNextValue;

	if(TransitionRatio > 1.0f )
	{
		TransitionRatio = 1.0f;
	}
	if(TransitionRatio < 0.0f )
	{
		TransitionRatio = 0.0f;
	}
	if(std::isnan(TransitionRatio))
	{
		return *CurrentValue;
	}

	return Interpolate(*CurrentValue, PendingTimeStampedValue.second, TransitionRatio);
}

template <typename TValue>
void DelayedLinearInterpolator<TValue>::ApplyTimeDriftCorrection(float Timestamp)
{
	float TimeToTargetBeforeCorrection = Timestamp - CurrentTime;
	if(TimeToTargetBeforeCorrection > InterpolationDelay)
	{
		CurrentTime += TimeToTargetBeforeCorrection * EasingFactor;
	}
}

template <typename TValue>
template <typename ... Args>
void DelayedLinearInterpolator<TValue>::EnqueueNewValue(float AbsoluteTime, Args&&... args)
{
	InterpolationValues.emplace(TimeStampedValue{AbsoluteTime, TValue{ std::forward<Args>(args)... }});
}

template <typename TValue>
bool DelayedLinearInterpolator<TValue>::IsCurrentTimeAheadOf(float AbsoluteTime) const
{
	return CurrentTime > AbsoluteTime;
}

template <typename TValue>
bool DelayedLinearInterpolator<TValue>::HasPendingValues()
{
	return !InterpolationValues.empty();
}

template <typename TValue>
void DelayedLinearInterpolator<TValue>::ClearValue(TValue* ValuePtr)
{
    if(ValuePtr == nullptr)
    {
        return;
    }
    ValuePtr->~TValue();
    ValuePtr = nullptr;
}

    template <typename TValue>
template <typename ... Args>
void DelayedLinearInterpolator<TValue>::SetValue(TValue** ValuePtr, TValueStore* ValueStorage, Args&&... ContructorArgs)
{
    ClearValue(*ValuePtr);
	*ValuePtr = new(ValueStorage) TValue{ std::forward<Args>(ContructorArgs)... };
}

template <typename TValue>
template <typename ... Args>
void DelayedLinearInterpolator<TValue>::SetCurrentValue(Args&&... ContructorArgs)
{
    SetValue(&CurrentValue, &CurrentValueLocation, std::forward<Args>(ContructorArgs)...);
}

template <typename TValue>
template <typename ... Args>
void DelayedLinearInterpolator<TValue>::SetPreviousValue(Args&&... ContructorArgs)
{
    SetValue(&PreviousValue, &PreviousValueLocation, std::forward<Args>(ContructorArgs)...);
}
}  // ::unreal
}  // ::improbable