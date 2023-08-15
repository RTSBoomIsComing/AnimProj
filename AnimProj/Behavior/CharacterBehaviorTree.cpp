#include "pch.h"
#include "CharacterBehaviorTree.h"
#include "../World/World.h"
#include "../Actor/Actor.h"
#include "../Map/GridMap.h"
#include "../Component/SceneComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/AnimationComponent2.h"

#include "../Animation/AnimationManager.h"
namespace pa
{
	CharacterBehaviorTree::CharacterBehaviorTree()
	{
		auto rootSequence = std::make_shared<Behavior::Sequence>();
		_root			  = rootSequence;

		auto findTarget = std::make_shared<FindTarget>();
		auto Attack		= std::make_shared<Behavior::Sequence>();
		Attack->addChild(std::make_shared<PlayAnimationUpperBody>(
			AnimationManager::get().getAnimation(AnimationManager::AnimationIndex::ShootingGun_up)));

		Attack->addChild(std::make_shared<PlayAnimationLowerBody>(
			AnimationManager::get().getAnimation(AnimationManager::AnimationIndex::ShootingGun_lo)));

		rootSequence->addChild(findTarget);
		rootSequence->addChild(Attack);

		// findTarget->addChild(Attack);

		// rootSequence->addChild(findTarget);

		// rootSequence->addChild(std::make_shared<PlayAnimationLowerBody>(
		//	AnimationManager::get().getAnimation(AnimationManager::AnimationIndex::Walk_lo)));

		// rootSequence->addChild(std::make_shared<PlayAnimationUpperBody>(
		//	AnimationManager::get().getAnimation(AnimationManager::AnimationIndex::ShootingGun_up)));
	}

	bool CharacterBehaviorTree::FindTarget::onUpdate(World& world, Actor& owner)
	{
		using namespace DirectX;

		constexpr float _radius = 100.0f;

		SceneComponent*			 sceneComp = owner.getComponent<SceneComponent>();
		assert(sceneComp);

		std::shared_ptr<GridMap> map	   = world.getDefaultMap();

		std::pair<size_t, size_t> cellCoordinate = map->getCellCoordinate(world, owner);
		std::vector<Actor*>&	  actors		 = map->getCell(cellCoordinate.first, cellCoordinate.second);

		XMVECTOR V0 = XMLoadFloat3(&sceneComp->position);
		for (const Actor* other : actors)
		{
			assert(other);

			const SceneComponent* otherSceneComp = other->getComponent<SceneComponent>();
			assert(otherSceneComp);

			XMVECTOR	V1		 = XMLoadFloat3(&otherSceneComp->position);
			const float distance = XMVectorGetX(XMVector3Length(V1 - V0));

			MovementComponent* movementComp = owner.getComponent<MovementComponent>();
			assert(movementComp);

			movementComp->speed = 0.0f;
			if (distance < _radius && 0 < distance)
			{
				XMStoreFloat3(&movementComp->targetPosition, V1);
				movementComp->speed = 1.0f;

				return true;
			}
		}
		return false;
	}

	bool CharacterBehaviorTree::PlayAnimationLowerBody::onUpdate(World& world, Actor& owner)
	{
		AnimationComponent2* animationComp = owner.getComponent<AnimationComponent2>();
		assert(animationComp);


		if (animationComp->getCurrentAnimationLowerBody() == _animation)
			return false;

		animationComp->transitAnimationLowerBody(*_animation, 0.1f);

		return true;
	}

	bool CharacterBehaviorTree::PlayAnimationUpperBody::onUpdate(World& world, Actor& owner)
	{
		AnimationComponent2* animationComp = owner.getComponent<AnimationComponent2>();
		assert(animationComp);


		if (animationComp->getCurrentAnimationUpperBody() == _animation)
			return false;

		animationComp->transitAnimationUpperBody(*_animation, 0.1f);

		return true;
	}

	bool CharacterBehaviorTree::MoveToCenter::onUpdate(World& world, Actor& owner)
	{
		MovementComponent* movementComp = owner.getComponent<MovementComponent>();
		assert(movementComp);

		movementComp->targetPosition = {0.0f, 0.0f, 0.0f};

		return false;
	}

}
