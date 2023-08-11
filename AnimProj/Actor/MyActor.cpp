// author: Changwan Yu
#include "pch.h"
#include "MyActor.h"
#include "../World/World.h"
#include "../Component/SceneComponent.h"
#include "../Component/BehaviorTreeComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/SkeletalMeshComponent.h"

#include "../Behavior/CharacterBehaviorTree.h"
#include "../Animation/AnimationManager.h"

void pa::MyActor::initializeComponents(World& world)
{
	initializeComponent<SceneComponent>(world);
	initializeComponent<MovementComponent>(world);

	initializeComponent<BehaviorTreeComponent>(world);
	std::shared_ptr<BehaviorTree> behaviorTree = std::make_shared<CharacterBehaviorTree>();
	getComponent<BehaviorTreeComponent>(world).setBehaviorTree(behaviorTree);

	initializeComponent<SkeletalMeshComponent>(world);
	getComponent<SkeletalMeshComponent>(world).setSkeleton(AnimationManager::get().getDefaultSkeleton());
}

void pa::MyActor::onStartGame(World& world)
{
}

void pa::MyActor::onUpdate(World& world, float deltaTime)
{
}
