#include "pch.h"
#include "CharacterBehaviorTree.h"
#include "../World/World.h"
#include "../Actor/Actor.h"
#include "../Map/GridMap.h"
#include "../Component/SceneComponent.h"
#include "../Component/MovementComponent.h"

namespace pa
{
	CharacterBehaviorTree::CharacterBehaviorTree()
	{
		std::shared_ptr<Behavior::Selector> rootSequence = std::make_shared<Behavior::Selector>();
		_root											 = rootSequence;

		rootSequence->addChild(std::make_shared<FindTarget>());
		rootSequence->addChild(std::make_shared<MoveToTarget>());
	}

	bool CharacterBehaviorTree::FindTarget::onUpdate(World& world, Actor& owner)
	{
		using namespace DirectX;

		constexpr float _radius = 20.0f;

		SceneComponent*			 sceneComp = owner.getComponent<SceneComponent>();
		std::shared_ptr<GridMap> map			= world.getDefaultMap();

		auto  cellCoordinate = map->getCellCoordinate(world, owner);
		auto& actors		 = map->getCell(cellCoordinate.first, cellCoordinate.second);

		XMVECTOR V0 = XMLoadFloat3(&sceneComp->position);
		for (const Actor* other : actors)
		{
			assert(other);

			const SceneComponent* otherSceneComp = other->getComponent<SceneComponent>();
			XMVECTOR			  V1 = XMLoadFloat3(&otherSceneComp->position);
			const float			  distance		 = XMVectorGetX(XMVector3Length(V1 - V0));

			MovementComponent* movementComp = owner.getComponent<MovementComponent>();
			movementComp->speed				= 0.0f;
			if (distance < _radius)
			{
				XMStoreFloat3(&movementComp->targetPosition, V1);
				movementComp->speed = 1.0f;
				return true;
			}
		}
		return false;
	}

	bool CharacterBehaviorTree::MoveToTarget::onUpdate(World& world, Actor& owner)
	{
		//MovementComponent& movementComp = ownerLocked->getComponent<MovementComponent>(world);

		return true;
	}

}
