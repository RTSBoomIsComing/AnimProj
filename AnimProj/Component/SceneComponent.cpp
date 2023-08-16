// author: Changwan Yu
#include "pch.h"
#include "SceneComponent.h"
#include "../World/World.h"
#include "../Actor/Actor.h"
void pa::SceneComponent::onUpdate(World& world, Actor& owner, float deltaTime)
{
	if (!owner.isAlive())
	{
		position.y = position.y - 5.0f * deltaTime;
		position.y = std::max(position.y, -5.0f);
	}
}
