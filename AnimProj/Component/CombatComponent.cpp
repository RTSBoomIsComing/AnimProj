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

	SceneComponent* sceneComp = owner.getComponent<SceneComponent>();
	assert(sceneComp);

	MovementComponent* movementComp = owner.getComponent<MovementComponent>();
	assert(movementComp);

	if (!owner.isAlive())
		return;

	if (nullptr == _targetToAttack)
	{
		_isAttacking = false;
		_attackTimer = _attackPreparationTime;
		movementComp->setMovable(true);
		return;
	}

	if (!_targetToAttack->isAlive())
	{
		_targetToAttack = nullptr;
		_isAttacking	= false;
		_attackTimer	= _attackPreparationTime;
		movementComp->setMovable(true);
		return;
	}

	SceneComponent* targetSceneComp = _targetToAttack->getComponent<SceneComponent>();
	assert(targetSceneComp);

	const XMVECTOR V0				= XMLoadFloat3(&sceneComp->position);
	const XMVECTOR V1				= XMLoadFloat3(&targetSceneComp->position);
	float		   distanceToTarget = XMVectorGetX(XMVector3Length(V1 - V0));

	if (distanceToTarget > _attackRange)
	{
		_targetToAttack = nullptr;
		_isAttacking	= false;
		_attackTimer	= _attackPreparationTime;
		movementComp->setMovable(true);
		return;
	}

	// start attacking
	// orientate to target to attack
	movementComp->targetPosition = targetSceneComp->position;

	// disable movement, just enable rotation
	movementComp->setMovable(false);
	_isAttacking = true;

	if (_attackTimer > 0.0f)
	{
		_attackTimer -= deltaTime;
		if (_attackTimer <= 0.0f)
		{
			applyDamage(world, owner);
		}
	}
}

void pa::CombatComponent::onEndAttack()
{
	_attackTimer = _attackPreparationTime;
}

void pa::CombatComponent::setTargetToAttack(Actor* target)
{
	_targetToAttack = target;
}

void pa::CombatComponent::applyDamage(World& world, Actor& owner)
{
	CombatComponent* targetCombatComp = _targetToAttack->getComponent<CombatComponent>();
	assert(targetCombatComp);

	targetCombatComp->_health -= _attackDamage;
	if (targetCombatComp->_health <= 0.0f)
	{
		_targetToAttack->setAlive(false);
	}
}
