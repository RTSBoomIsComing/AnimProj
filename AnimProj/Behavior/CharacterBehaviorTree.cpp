#include "pch.h"
#include "CharacterBehaviorTree.h"
#include "../World/World.h"
#include "../Actor/Actor.h"
#include "../Map/GridMap.h"
#include "../Component/SceneComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/AnimationComponent.h"
namespace pa
{
	CharacterBehaviorTree::CharacterBehaviorTree()
	{
		std::shared_ptr<Behavior::Composite> rootSequence = std::make_shared<Behavior::Sequence>();
		_root											  = rootSequence;

		rootSequence->addChild(std::make_shared<FindTarget>());
	}

	bool CharacterBehaviorTree::FindTarget::onUpdate(World& world, Actor& owner)
	{
		using namespace DirectX;

		constexpr float _radius = 20.0f;

		SceneComponent*			 sceneComp = owner.getComponent<SceneComponent>();
		std::shared_ptr<GridMap> map	   = world.getDefaultMap();

		auto  cellCoordinate = map->getCellCoordinate(world, owner);
		auto& actors		 = map->getCell(cellCoordinate.first, cellCoordinate.second);

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
			if (distance < _radius)
			{
				XMStoreFloat3(&movementComp->targetPosition, V1);
				movementComp->speed = 1.0f;

				{
					const XMVECTOR Vdir	   = XMVector3Normalize(V1 - V0);
					const XMVECTOR Forward = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
					const XMVECTOR Axis	   = XMVector3Cross(Forward, Vdir);
					const float	   dot	   = XMVectorGetX(XMVector3Dot(Forward, Vdir));
					float		   angle   = std::acosf(dot);

					if (XMVectorGetY(Axis) < 0.0f)
						angle = -1.0f * angle;

					movementComp->targetEulerAngle.y = angle;
				}

				return true;
			}
		}
		return false;
	}

	bool CharacterBehaviorTree::PlayAnimation::onUpdate(World& world, Actor& owner)
	{
		// AnimationComponent* animationComp = owner.getComponent<AnimationComponent>();
		// animationComp->setAnimation("Idle");
		// animationComp->playAnimation("Idle");
		SkeletalMeshComponent* skeletalMeshComp = owner.getComponent<SkeletalMeshComponent>();
		//skeletalMeshComp->playAnimation("Idle");


		return false;
	}

}
