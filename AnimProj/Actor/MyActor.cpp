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

pa::MyActor::MyActor()
{
	_sceneComponent = this->createDefaultComponent<SceneComponent>();
	_movementComponent = this->createDefaultComponent<MovementComponent>();

	_behaviorTreeComponent = this->createDefaultComponent<BehaviorTreeComponent>();
	_behaviorTreeComponent->setBehaviorTree(std::make_shared<CharacterBehaviorTree>());

	_skeletalMeshComponent = this->createDefaultComponent<SkeletalMeshComponent>();
	_skeletalMeshComponent->setSkeleton(AnimationManager::get().getDefaultSkeleton());
}

void pa::MyActor::onStartGame(World& world)
{
	Actor::onStartGame(world);
}

void pa::MyActor::onUpdate(World& world, float deltaTime)
{
	Actor::onUpdate(world, deltaTime);

	_behaviorTreeComponent->onUpdate(world, *this, deltaTime);
	_movementComponent->onUpdate(world, *this, deltaTime);
	_skeletalMeshComponent->onUpdate(world, *this, deltaTime);
}
