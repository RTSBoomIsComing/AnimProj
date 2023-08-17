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
	static bool isEnemy(int teamID, const Actor* actor)
	{
		const CombatComponent* combatComp = actor->getComponent<CombatComponent>();
		assert(combatComp);

		return combatComp->getTeamID() != teamID;
	}


	CharacterBehaviorTree::CharacterBehaviorTree(World& world)
	{
		auto findTargetInAttackRange = std::make_shared<FindTargetInAttackRange>();
		auto findTargetInSight		 = std::make_shared<FindTargetInSight>();
		auto isAttacking			 = std::make_shared<IsAttacking>();
		auto isTargetInAttackRange	 = std::make_shared<IsTargetInAttackRange>();
		auto attack					 = std::make_shared<Attack>();
		auto moveToCenter			 = std::make_shared<MoveToCenter>();


		auto mainSelector = std::make_shared<Behavior::Selector>();
		mainSelector->addChild(isAttacking);

		auto attackSequence = std::make_shared<Behavior::Sequence>();

		auto setTargetToAttack = std::make_shared<Behavior::Selector>();
		setTargetToAttack->addChild(isTargetInAttackRange);
		setTargetToAttack->addChild(findTargetInAttackRange);

		mainSelector->addChild(attackSequence);
		attackSequence->addChild(setTargetToAttack);
		attackSequence->addChild(attack);

		mainSelector->addChild(findTargetInSight);
		mainSelector->addChild(moveToCenter);

		_root = mainSelector;
	}

	bool CharacterBehaviorTree::MoveToCenter::onUpdate(World& world, Actor& owner)
	{
		MovementComponent* movementComp = owner.getComponent<MovementComponent>();
		assert(movementComp);

		std::pair<float, float> mapCenterXZ = world.getDefaultMap()->getMapCenter();
		movementComp->targetPosition.x		= mapCenterXZ.first;
		movementComp->targetPosition.z		= mapCenterXZ.second;

		return true;
	}
	bool CharacterBehaviorTree::FindTargetInAttackRange::onUpdate(World& world, Actor& owner)
	{
		using namespace DirectX;

		SceneComponent* sceneComp = owner.getComponent<SceneComponent>();
		assert(sceneComp);

		CombatComponent* combatComp = owner.getComponent<CombatComponent>();
		assert(combatComp);

		std::shared_ptr<GridMap> map = world.getDefaultMap();

		std::function<bool(const Actor*)> predicate = std::bind(&isEnemy, combatComp->getTeamID(), std::placeholders::_1);
		Actor*							  other		= map->findNearestActor(world, owner, combatComp->getAttackRange(), predicate);
		/*Actor*							  other		= map->findNearestActor(world, owner, combatComp->getAttackRange());*/

		if (nullptr == other)
		{
			combatComp->setTargetToAttack(nullptr);
			return false;
		}
		combatComp->setTargetToAttack(other);

		MovementComponent* movementComp = owner.getComponent<MovementComponent>();
		assert(movementComp);

		SceneComponent* otherSceneComp = other->getComponent<SceneComponent>();
		assert(otherSceneComp);
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

		if (combatComp->isTargetValid())
			return true;

		std::shared_ptr<GridMap>		  map		= world.getDefaultMap();
		std::function<bool(const Actor*)> predicate = std::bind(&isEnemy, combatComp->getTeamID(), std::placeholders::_1);
		Actor*							  other		= map->findNearestActor(world, owner, combatComp->getSightRange(), predicate);

		if (nullptr == other)
		{
			combatComp->setTargetToAttack(nullptr);
			return false;
		}

		combatComp->setTargetToAttack(other);

		MovementComponent* movementComp = owner.getComponent<MovementComponent>();
		assert(movementComp);

		SceneComponent* otherSceneComp = other->getComponent<SceneComponent>();
		assert(otherSceneComp);
		movementComp->targetPosition = otherSceneComp->position;


		return true;
	}

	bool CharacterBehaviorTree::Attack::onUpdate(World& world, Actor& owner)
	{
		CombatComponent* combatComp = owner.getComponent<CombatComponent>();
		assert(combatComp);

		if (false == combatComp->isTargetValid())
			return false;

		return combatComp->startAttack();
	}
	bool CharacterBehaviorTree::IsTargetInAttackRange::onUpdate(World& world, Actor& owner)
	{
		CombatComponent* combatComp = owner.getComponent<CombatComponent>();
		assert(combatComp);

		if (false == combatComp->isTargetValid())
			return false;

		SceneComponent* sceneComp = owner.getComponent<SceneComponent>();
		assert(sceneComp);

		SceneComponent* otherSceneComp = combatComp->getTargetToAttack()->getComponent<SceneComponent>();
		assert(otherSceneComp);

		using namespace DirectX;
		const XMVECTOR V0		= XMLoadFloat3(&sceneComp->position);
		const XMVECTOR V1		= XMLoadFloat3(&otherSceneComp->position);
		const float	   distance = XMVectorGetX(XMVector3Length(V1 - V0));

		if (distance <= combatComp->getAttackRange())
			return true;

		return false;
	}
	bool CharacterBehaviorTree::IsAttacking::onUpdate(World& world, Actor& owner)
	{
		CombatComponent* combatComp = owner.getComponent<CombatComponent>();
		assert(combatComp);

		if (combatComp->isAttacking())
			return true;

		return false;
	}

	bool CharacterBehaviorTree::MoveAcrossMap::onUpdate(World& world, Actor& owner)
	{
		SceneComponent* sceneComp = owner.getComponent<SceneComponent>();
		assert(sceneComp);

		std::pair<float, float> mapCenterXZ = world.getDefaultMap()->getMapCenter();
		if (mapCenterXZ.first == sceneComp->position.x)
			return false;

		MovementComponent* movementComp = owner.getComponent<MovementComponent>();
		assert(movementComp);


		movementComp->targetPosition.x = mapCenterXZ.first;
		movementComp->targetPosition.z = sceneComp->position.z;

		return true;
	}
}
