// author: Changwan Yu
#include "pch.h"
#include "CombatComponent.h"
#include "../Actor/Actor.h"
#include "../World/World.h"
#include "../Component/SceneComponent.h"
#include "../Component/MovementComponent.h"

void pa::CombatComponent::onUpdate(World& world, Actor& owner, float deltaTime)
{
	using namespace DirectX;

	if (!owner.isAlive())
		return;

	SceneComponent* sceneComp = owner.getComponent<SceneComponent>();
	assert(sceneComp);

	MovementComponent* movementComp = owner.getComponent<MovementComponent>();
	assert(movementComp);


	if (nullptr == _targetToAttack)
	{
		_isAttacking = false;
		return;
	}

	if (!_targetToAttack->isAlive())
	{
		_targetToAttack = nullptr;
		_isAttacking	= false;
		return;
	}


	SceneComponent* targetSceneComp = _targetToAttack->getComponent<SceneComponent>();
	assert(targetSceneComp);

	const XMVECTOR V0				= XMLoadFloat3(&sceneComp->position);
	const XMVECTOR V1				= XMLoadFloat3(&targetSceneComp->position);
	float		   distanceToTarget = XMVectorGetX(XMVector3Length(V1 - V0));

	if (!_isAttacking && distanceToTarget <= _attackRange)
	{
		_isAttacking = true;
	}
}

void pa::CombatComponent::onEndAttack()
{
	_isAttacking = false;
}

void pa::CombatComponent::onAttack()
{
	if (nullptr == _targetToAttack)
		return;

	CombatComponent* targetCombatComp = _targetToAttack->getComponent<CombatComponent>();
	assert(targetCombatComp);

	targetCombatComp->_health -= _attackDamage;
	if (targetCombatComp->_health <= 0.0f)
	{
		_targetToAttack->setAlive(false);
	}
}

void pa::CombatComponent::setTargetToAttack(Actor* target)
{
	_targetToAttack = target;
}

