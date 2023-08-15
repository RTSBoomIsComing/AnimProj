// author: Changwan Yu
#include "pch.h"
#include "MyActor.h"
#include "../World/World.h"
#include "../Animation/AnimationManager.h"
#include "../Behavior/CharacterBehaviorTree.h"

#include "../Component/SceneComponent.h"
#include "../Component/BehaviorTreeComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/SkeletalMeshComponent.h"
#include "../Component/AnimationComponent2.h"
#include "../Component/CombatComponent.h"

namespace pa
{
	MyActor::MyActor()
	{
		_sceneComponent	   = this->createDefaultComponent<SceneComponent>();
		_movementComponent = this->createDefaultComponent<MovementComponent>();

		_behaviorTreeComponent = this->createDefaultComponent<BehaviorTreeComponent>();
		//_behaviorTreeComponent->setBehaviorTree(std::make_shared<CharacterBehaviorTree>());

		_skeletalMeshComponent = this->createDefaultComponent<SkeletalMeshComponent>();
		_skeletalMeshComponent->setSkeleton(AnimationManager::get().getDefaultSkeleton());

		_animationComponent = this->createDefaultComponent<AnimationComponent2>();

		_combatComponent = this->createDefaultComponent<CombatComponent>();
	}

	void MyActor::onStartGame(World& world)
	{
		Actor::onStartGame(world);
	}

	void MyActor::onUpdate(World& world, float deltaTime)
	{
		Actor::onUpdate(world, deltaTime);
		{
			// TODO: Move these to Actor::onUpdate()
			_behaviorTreeComponent->onUpdate(world, *this, deltaTime);
			_combatComponent->onUpdate(world, *this, deltaTime);
			_movementComponent->onUpdate(world, *this, deltaTime);
			_skeletalMeshComponent->onUpdate(world, *this, deltaTime);
			_animationComponent->onUpdate(world, *this, deltaTime);
		}
	}
}