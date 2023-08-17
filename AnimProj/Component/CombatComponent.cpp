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
}

bool pa::CombatComponent::startAttack()
{
	if (nullptr == _targetToAttack || _targetToAttack->isAlive())
		_isAttacking = false;

	_isAttacking = true;
	_hasBeenAttacked = false;
	return _isAttacking;
}

void pa::CombatComponent::onEndAttack()
{
	_isAttacking = false;
}

void pa::CombatComponent::onAttack()
{
	if (nullptr == _targetToAttack)
		return;

	if (_hasBeenAttacked)
		return;

	_hasBeenAttacked = true;
	CombatComponent* targetCombatComp = _targetToAttack->getComponent<CombatComponent>();
	assert(targetCombatComp);

	targetCombatComp->_health -= _attackDamage;
	if (targetCombatComp->_health <= 0.0f)
	{
		_targetToAttack->setAlive(false);
	}
}

bool pa::CombatComponent::isTargetValid() const
{
	if (nullptr == _targetToAttack)
		return false;

	if (!_targetToAttack->isAlive())
		return false;

	return true;
}

void pa::CombatComponent::setTargetToAttack(Actor* target)
{
	_targetToAttack = target;
}
