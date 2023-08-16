#include "pch.h"
#include "CharacterBehaviorTree.h"
#include "../World/World.h"
#include "../Actor/Actor.h"
#include "../Map/GridMap.h"
#include "../Animation/AnimationManager.h"

#include "../Component/SceneComponent.h"
#include "../Component/MovementComponent.h"
#include "../Component/AnimationComponent2.h"
#include "../Component/CombatComponent.h"

namespace pa
{
	CharacterBehaviorTree::CharacterBehaviorTree(World& world)
	{
		auto findTargetOnSight		 = std::make_shared<FindTarget>(20.0f);

		std::pair<float, float> mapCenterXZ = world.getDefaultMap()->getMapCenter();
		DirectX::XMFLOAT3		mapCenterPosition(mapCenterXZ.first, 0.0f, mapCenterXZ.second);
		auto moveToCenter = std::make_shared<MoveTo>(mapCenterPosition);

		auto mainSelector = std::make_shared<Behavior::Selector>();
		mainSelector->addChild(findTargetOnSight);
		mainSelector->addChild(moveToCenter);

		_root = mainSelector;
	}

	bool CharacterBehaviorTree::FindTarget::onUpdate(World& world, Actor& owner)
	{
		using namespace DirectX;

		SceneComponent* sceneComp = owner.getComponent<SceneComponent>();
		assert(sceneComp);

		CombatComponent* combatComp = owner.getComponent<CombatComponent>();
		assert(combatComp);

		if (combatComp->hasTarget())
			return true;

		std::shared_ptr<GridMap> map   = world.getDefaultMap();
		Actor*					 other = map->findNearestActor(world, owner, _radius);

		if (nullptr == other)
		{
			combatComp->setTargetToAttack(nullptr);
			return false;
		}

		combatComp->setTargetToAttack(other);

		return true;
	}

	bool CharacterBehaviorTree::MoveTo::onUpdate(World& world, Actor& owner)
	{
		MovementComponent* movementComp = owner.getComponent<MovementComponent>();
		assert(movementComp);

		movementComp->targetPosition = _position;
		movementComp->setMovable(true);

		return true;
	}
}
