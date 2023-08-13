// author: Changwan Yu
#include "pch.h"
#include "BehaviorTreeComponent.h"
#include "../Behavior/BehaviorTree.h"

void pa::BehaviorTreeComponent::onUpdate(World& world, Actor& owner, float deltaTime)
{
	const bool result = _behaviorTree->getRoot()->onUpdate(world, owner);
}
