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
		auto findTargetInAttackRange = std::make_shared<FindTargetInAttackRange>();

		std::pair<float, float> mapCenterXZ = world.getDefaultMap()->getMapCenter();
		DirectX::XMFLOAT3		mapCenterPosition(mapCenterXZ.first, 0.0f, mapCenterXZ.second);
		auto moveToCenter = std::make_shared<MoveTo>(mapCenterPosition);

		auto mainSelector = std::make_shared<Behavior::Selector>();
		mainSelector->addChild(findTargetInAttackRange);
		mainSelector->addChild(moveToCenter);

		_root = mainSelector;
	}

	bool CharacterBehaviorTree::MoveTo::onUpdate(World& world, Actor& owner)
	{
		MovementComponent* movementComp = owner.getComponent<MovementComponent>();
		assert(movementComp);

		movementComp->targetPosition = _position;

		return true;
	}
	bool CharacterBehaviorTree::FindTargetInAttackRange::onUpdate(World& world, Actor& owner)
	{
		using namespace DirectX;

		SceneComponent* sceneComp = owner.getComponent<SceneComponent>();
		assert(sceneComp);

		MovementComponent* movementComp = owner.getComponent<MovementComponent>();
		assert(movementComp);

		CombatComponent* combatComp = owner.getComponent<CombatComponent>();
		assert(combatComp);

		if (combatComp->hasTarget())
			return true;

		std::shared_ptr<GridMap> map   = world.getDefaultMap();
		Actor*					 other = map->findNearestActor(world, owner, combatComp->getAttackRange());

		if (nullptr == other)
		{
			combatComp->setTargetToAttack(nullptr);
			return false;
		}
		combatComp->setTargetToAttack(other);

		SceneComponent* otherSceneComp = other->getComponent<SceneComponent>();
		movementComp->targetPosition = otherSceneComp->position;

		return true;
	}
	bool CharacterBehaviorTree::FindTargetInSight::onUpdate(World& world, Actor& owner)
	{
		using namespace DirectX;

		SceneComponent* sceneComp = owner.getComponent<SceneComponent>();
		assert(sceneComp);

		CombatComponent* combatComp = owner.getComponent<CombatComponent>();
		assert(combatComp);

		if (combatComp->hasTarget())
			return true;

		std::shared_ptr<GridMap> map   = world.getDefaultMap();
		Actor*					 other = map->findNearestActor(world, owner, combatComp->getSightRange());

		if (nullptr == other)
		{
			combatComp->setTargetToAttack(nullptr);
			return false;
		}

		combatComp->setTargetToAttack(other);

		return true;
	}
}
