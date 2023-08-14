// author: Changwan Yu
#pragma once
#include "Actor.h"
namespace pa
{
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
		AnimationComponent2*   _animationComponent;
	};
}
