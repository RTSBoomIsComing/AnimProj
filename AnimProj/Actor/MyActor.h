// author: Changwan Yu
#pragma once
#include "Actor.h"
#include "../Component/Component.h"
namespace pa
{
	class SceneComponent;
	class MovementComponent;
	class BehaviorTreeComponent;
	class SkeletalMeshComponent;

	class MyActor : public Actor
	{
	public:
		MyActor();
		//virtual void initializeComponents(World& world) override;
		virtual void onStartGame(World& world) override;
		virtual void onUpdate(World& world, float deltaTime) override;

	private:
		SceneComponent*		   _sceneComponent;
		MovementComponent*	   _movementComponent;
		BehaviorTreeComponent* _behaviorTreeComponent;
		SkeletalMeshComponent* _skeletalMeshComponent;
	};
}
