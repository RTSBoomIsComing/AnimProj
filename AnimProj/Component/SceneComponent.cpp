// author: Changwan Yu
#include "pch.h"
#include "SceneComponent.h"
#include "../World/World.h"
#include "../Actor/Actor.h"
void pa::SceneComponent::onUpdate(World& world, Actor& owner, float deltaTime)
{
	if (_isPendingKill)
		return;

	if (!owner.isAlive())
	{
		_killTimer += deltaTime;
		if (_killTimer >= 3.0f)
		{
			_isPendingKill = true;
		}

		position.y = position.y - 5.0f * deltaTime;
		position.y = std::max(position.y, -5.0f);
	}
}
