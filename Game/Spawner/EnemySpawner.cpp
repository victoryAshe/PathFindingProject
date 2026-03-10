#include "Spawner/EnemySpawner.h"

#include "Level/IngameLevel.h"

EnemySpawner::EnemySpawner(IngameLevel* owningLevel)
	: owningLevel(owningLevel)
{
}

void EnemySpawner::Tick(float deltaTime)
{
	if (!isSpawningEnabled)
	{
		return;
	}

	spawnTimer.Tick(deltaTime);

	if (!spawnTimer.IsTimeOut())
	{
		return;
	}

	TrySpawnEnemy();
	spawnTimer.Restart(spawnInterval);
}

void EnemySpawner::Start()
{
	isSpawningEnabled = true;
	spawnTimer.Restart(spawnInterval);
}

void EnemySpawner::Stop()
{
	isSpawningEnabled = false;
}

void EnemySpawner::SetSpawnInterval(float newSpawnInterval)
{
	if (newSpawnInterval <= 0.0f)
	{
		return;
	}

	spawnInterval = newSpawnInterval;
	spawnTimer.Restart(spawnInterval);
}

float EnemySpawner::GetSpawnInterval() const
{
	return spawnInterval;
}

void EnemySpawner::SetSpawningEnabled(bool newIsSpawningEnabled)
{
	isSpawningEnabled = newIsSpawningEnabled;
}

bool EnemySpawner::IsSpawningEnabled() const
{
	return isSpawningEnabled;
}

void EnemySpawner::TrySpawnEnemy()
{
	if (!owningLevel)
	{
		return;
	}

	owningLevel->SpawnEnemyAtRandomLocation();
}