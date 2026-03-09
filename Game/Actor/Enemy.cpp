#include "Enemy.h"

#include "Level/IngameLevel.h"
#include "Util/Util.h"

Enemy::Enemy(const Vector2& newPosition)
	: super("E", newPosition, Color::Blue)
{
	sortingOrder = EnemySortingOrder;

	hp = Util::Random(1, 5);
	
	ChangeImage(std::to_string(hp).c_str());
}

void Enemy::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	Move(deltaTime);
	DrawCurrentPath();
}

void Enemy::SetOwner(Level* newOwner)
{
	super::SetOwner(newOwner);
	owningIngameLevel = dynamic_cast<IngameLevel*>(newOwner);
}

bool Enemy::CanRequestPath(float deltaTime)
{
	if (repathCooldownSeconds > 0.0f)
	{
		repathCooldownSeconds -= deltaTime;
		if (repathCooldownSeconds < 0.0f)
		{
			repathCooldownSeconds = 0.0f;
		}
	}

	return repathCooldownSeconds <= 0.0f;
}

void Enemy::ResetNavigationState()
{
	hasCachedApproachPosition = false;
	cachedApproachPosition = Vector2(-1, -1);

	cachedPath.clear();
	nextPathIndex = 1;

	moveStepAccumulator = 0.0f;
}

bool Enemy::RequestPath(IngameLevel* level)
{
	if (!level || !level->GetPlayerActor())
	{
		return false;
	}

	Vector2 selectedApproachPosition(-1, -1);

	std::vector<Vector2> newPath = level->FindPathToActor(
		GetPosition(),
		level->GetPlayerActor()->GetPosition(),
		attackRange,
		hasCachedApproachPosition ? &cachedApproachPosition : nullptr,
		&selectedApproachPosition
	);

	if (newPath.empty())
	{
		ResetNavigationState();
		repathCooldownSeconds = blockedRepathIntervalSeconds;
		return false;
	}

	hasCachedApproachPosition = true;
	cachedApproachPosition = selectedApproachPosition;

	cachedPath = std::move(newPath);
	nextPathIndex = 1;
	repathCooldownSeconds = repathIntervalSeconds;

	// 이미 공격 가능 위치면 이동 path가 필요 없음.
	if (cachedPath.size() < 2)
	{
		moveStepAccumulator = 0.0f;
		cachedPath.clear();
		nextPathIndex = 1;
		return false;
	}

	return true;
}


void Enemy::Move(float deltaTime)
{
	if (!owningIngameLevel || !owningIngameLevel->GetPlayerActor())
	{
		return;
	}

	// 현재 따라갈 path가 없으면, 쿨다운이 끝났을 때만 새 path 요청.
	if (cachedPath.empty() || nextPathIndex >= cachedPath.size())
	{
		if (!CanRequestPath(deltaTime))
		{
			return;
		}

		const bool pathFound = RequestPath(owningIngameLevel);
		if (!pathFound)
		{
			return;
		}
	}
	else
	{
		// path가 남아 있더라도 쿨다운 타이머는 감소시켜 둠.
		if (repathCooldownSeconds > 0.0f)
		{
			repathCooldownSeconds -= deltaTime;
			if (repathCooldownSeconds < 0.0f)
			{
				repathCooldownSeconds = 0.0f;
			}
		}
	}

	// 이미 공격 가능 위치에 도달했으면 이동하지 않음.
	if (cachedPath.size() < 2 || nextPathIndex >= cachedPath.size())
	{
		moveStepAccumulator = 0.0f;
		return;
	}

	moveStepAccumulator += deltaTime;

	const float secondsPerTile = 1.0f / moveSpeed;
	if (moveStepAccumulator < secondsPerTile)
	{
		return;
	}

	moveStepAccumulator -= secondsPerTile;

	// 다음 칸으로 1칸 이동하기 전, 해당 칸이 점유되었는지 검사.
	const Vector2 nextPosition = cachedPath[nextPathIndex];
	if (!owningIngameLevel->CanMove(GetPosition(), nextPosition, GetSortingOrder()))
	{
		ResetNavigationState();

		repathCooldownSeconds = blockedRepathIntervalSeconds;
		return;
	}

	SetPosition(nextPosition);
	++nextPathIndex;

	// path를 다 소모했으면 다음 탐색은 조금 뒤에 하도록 둠.
	if (nextPathIndex >= cachedPath.size())
	{
		cachedPath.clear();
		nextPathIndex = 1;
		repathCooldownSeconds = repathIntervalSeconds;
	}
}

void Enemy::DrawCurrentPath() const
{
	if (!owningIngameLevel || cachedPath.empty() || nextPathIndex >= cachedPath.size()) return;

	if (nextPathIndex <= 1)
	{
		owningIngameLevel->DrawPath(cachedPath);
		return;
	}

	std::vector<Vector2> remainingPath(
		cachedPath.begin() + (nextPathIndex - 1),
		cachedPath.end()
	);

	owningIngameLevel->DrawPath(remainingPath);
}