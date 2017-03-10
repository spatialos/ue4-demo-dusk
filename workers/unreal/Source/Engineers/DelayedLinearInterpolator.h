#pragma once
#include <queue>
#include <memory>

namespace improbable
{
namespace unreal
{
template <typename TValue>
class DelayedLinearInterpolator 
{
public:
	DelayedLinearInterpolator();
	DelayedLinearInterpolator(float InterpolationDelay);
	DelayedLinearInterpolator(float InterpolationDelay, float EasingFactor);
	virtual ~DelayedLinearInterpolator();

	// copyable and moveable
	DelayedLinearInterpolator(const DelayedLinearInterpolator&) = default;
	DelayedLinearInterpolator(DelayedLinearInterpolator&&) = default;
	DelayedLinearInterpolator& operator=(const DelayedLinearInterpolator&) = default;
	DelayedLinearInterpolator& operator=(DelayedLinearInterpolator&&) = default;

	template <typename... Args>
	void Reset(float InitialValueAbsoluteTime, Args&&... args);

	template <typename... Args>
	void AddValue(float NewValueAbsoluteTime, Args&&... args);

	TValue GetInterpolatedValue(float DeltaTimeToAdvance);

	virtual TValue Interpolate(const TValue& FromValue, const TValue& ToValue, float ProgressRatio) = 0;

	bool HasValidOutput();

	void SetInterpolationDelay(float InterpolationDelaySeconds);

	void SetEasingFactor(float InEasingFactor);

private:
	using TimeStampedValue = std::pair<float, TValue>;
    using TValueStore = typename std::aligned_storage<sizeof(TValue), alignof(TValue)>::type;

	void DiscardOutdatedValues();

	TValue InterpolateToNextValue(float PreviousTime);

	void ApplyTimeDriftCorrection(float TimeStamp);

	template <typename... Args>
	void EnqueueNewValue(float AbsoluteTime, Args&&... args);

	bool IsCurrentTimeAheadOf(float AbsoluteTime) const;

	bool HasPendingValues();

    void ClearValue(TValue* ValuePtr);
	template <typename... Args>
    void SetValue(TValue** ValuePtr, TValueStore* ValueStorage, Args&&... ContructorArgs);
	template <typename... Args>
    void SetCurrentValue(Args&&... ContructorArgs);
	template <typename... Args>
    void SetPreviousValue(Args&&... ContructorArgs);

	const float DEFAULT_INTERPOLATION_DELAY_SECONDS = 0.1f;
	const float DEFAULT_EASING_FACTOR = 0.1f;

    std::queue<TimeStampedValue> InterpolationValues;

	TValue* PreviousValue;
	TValue* CurrentValue;

	TValueStore CurrentValueLocation;
	TValueStore PreviousValueLocation;

	float InterpolationDelay;
	float EasingFactor;

	bool bDelayChanged;

	float CurrentTime;
};
}  // ::unreal
}  // ::improbable
#include "DelayedLinearInterpolator.i.h"
