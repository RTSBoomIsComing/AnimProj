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

	bool CharacterBehaviorTree::FindTarget::onUpdate(World& world, std::weak_ptr<Actor> owner)
	{
		using namespace DirectX;

		constexpr float _radius = 20.0f;

		if (owner.expired())
			DebugBreak();

		auto ownerLocked = owner.lock();
		if (!ownerLocked)
			DebugBreak();

		auto&					 sceneComponent = ownerLocked->getComponent<SceneComponent>(world);
		std::shared_ptr<GridMap> map			= world.getDefaultMap();

		auto  cellCoordinate = map->getCellCoordinate(world, ownerLocked);
		auto& actors		 = map->getCell(cellCoordinate.first, cellCoordinate.second);

		XMVECTOR Vposition = XMLoadFloat3(&sceneComponent.position);
		for (auto& actor : actors)
		{
			if (actor.expired())
				DebugBreak();

			auto actorLocked = actor.lock();
			if (!actorLocked)
				DebugBreak();

			if (actorLocked == ownerLocked)
				continue;

			const SceneComponent& otherSceneComp = actorLocked->getComponent<SceneComponent>(world);
			XMVECTOR			  VotherPosition = XMLoadFloat3(&otherSceneComp.position);
			const float			  distance		 = XMVectorGetX(XMVector3Length(Vposition - VotherPosition));


			MovementComponent& movementComp = ownerLocked->getComponent<MovementComponent>(world);
			movementComp.speed				= 0.0f;
			if (distance < _radius)
			{
				XMStoreFloat3(&movementComp.targetPosition, VotherPosition);
				movementComp.speed = 1.0f;
				return true;
			}
		}
		return false;
	}

	bool CharacterBehaviorTree::MoveToTarget::onUpdate(World& world, std::weak_ptr<Actor> owner)
	{
		//MovementComponent& movementComp = ownerLocked->getComponent<MovementComponent>(world);

		return true;
	}

}
