// author: Changwan Yu
#include "pch.h"
#include "BehaviorTreeComponent.h"
#include "../Behavior/BehaviorTree.h"

void pa::BehaviorTreeComponent::update(float deltaTime)
{
	const bool result = _behaviorTree->getRoot()->run();
}
