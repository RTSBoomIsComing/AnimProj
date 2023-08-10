// author: Changwan Yu
#include "pch.h"
#include "MyActor.h"
#include "../World/World.h"
#include "../Component/SceneComponent.h"
#include "../Component/BehaviorTreeComponent.h"
#include "../Behavior/CharacterBehaviorTree.h"

void pa::MyActor::initializeComponents(World& world)
{
	initializeComponent<SceneComponent>(world);
	initializeComponent<BehaviorTreeComponent>(world);
}

void pa::MyActor::onStartGame(World& world)
{
	getComponent<SceneComponent>(world).position = { 1.0f, 1.0f, 1.0f };

	std::shared_ptr<BehaviorTree> behaviorTree = std::make_shared<CharacterBehaviorTree>();
	getComponent<BehaviorTreeComponent>(world).setBehaviorTree(behaviorTree);
}

void pa::MyActor::onUpdate(World& world, float deltaTime)
{
}
