// author: Changwan Yu
#include "pch.h"
#include "BehaviorTreeComponent.h"
#include "../Behavior/BehaviorTree.h"
#include "../Actor/Actor.h"

void pa::BehaviorTreeComponent::onUpdate(World& world, Actor& owner, float deltaTime)
{
	if (!owner.isAlive())
		return;

	const bool result = _behaviorTree->getRoot()->onUpdate(world, owner);
}
