#pragma once

#include "Util/Timer.h"

class IngameLevel;

class EnemySpawner
{
public:
	explicit EnemySpawner(IngameLevel* owningLevel = nullptr);

public:
	void Tick(float deltaTime);

	void Start();
	void Stop();

	void SetSpawnInterval(float newSpawnInterval);
	float GetSpawnInterval() const;

	void SetSpawningEnabled(bool newIsSpawningEnabled);
	bool IsSpawningEnabled() const;

private:
	void TrySpawnEnemy();

private:
	IngameLevel* owningLevel = nullptr;

	// 스폰 주기 값.
	float spawnInterval = 3.0f;

	// 주기 체크용 Timer.
	Timer spawnTimer = Timer(3.0f);

	// 스폰 활성화 여부.
	bool isSpawningEnabled = true;
};