#include "Timer.h"
#include <cmath>

Timer::Timer(float targetTime)
	: targetTime(targetTime)
{
}

void Timer::Tick(float deltaTime)
{
	// 경과 시간 업데이트.
	elapsedTime += deltaTime;
}

void Timer::Reset()
{
	elapsedTime = 0.0f;
}

bool Timer::IsTimeOut() const
{
	return elapsedTime >= targetTime;
}

void Timer::SetTargetTime(float newTargetTime)
{
	targetTime = newTargetTime;
}

void Timer::Restart(float newTargetTime)
{
	SetTargetTime(newTargetTime);
	Reset();
}

float Timer::GetRemainingTime() const
{
	// 음수로 내려가지 않게 처리.
	const float remainingTime = targetTime - elapsedTime;
	return remainingTime > 0.0f ? remainingTime : 0.0f;
}

float Timer::GetProgressRatio() const
{
	// Divide by Zero 방지.
	if (targetTime <= 0.0f)
	{
		return 1.0f;
	}

	const float ratio = elapsedTime / targetTime;

	
	if (ratio <= 0.0f)
	{
		return 0.0f;
	}

	if (ratio >= 1.0f)
	{
		return 1.0f;
	}

	return ratio;
}
