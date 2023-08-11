#include "pch.h"
#include "Character.h"
#include "../Animation/AnimationManager.h"

#include "../Component/SceneComponent.h"
#include "../Component/AnimationComponent.h"

#include <DirectXTK/Keyboard.h>
#include <DirectXTK/Mouse.h>

pa::Character::Character(ID3D11Device* device)
{
	const Skeleton& skeleton = AnimationManager::get().getDefaultSkeleton();


	_sceneComp		= std::make_shared<SceneComponent>();
	_animationComp	= std::make_shared<AnimationComponent>(device, skeleton);
}

pa::Character::Character(Character&& other) noexcept
{
	_moveTime		= other._moveTime;
	_jumpTime		= other._jumpTime;
	_attackTime		= other._attackTime;
	_isMoving		= other._isMoving;
	_isJumping		= other._isJumping;
	_isAttacking	= other._isAttacking;

	_skeletonComp	= other._skeletonComp;
	other._skeletonComp = nullptr;

	_sceneComp		= std::move(other._sceneComp);
	_animationComp	= std::move(other._animationComp);
}

pa::Character::~Character()
{
}

pa::Character& pa::Character::operator=(Character&& other) noexcept
{
	_moveTime = other._moveTime;
	_jumpTime = other._jumpTime;
	_attackTime = other._attackTime;
	_isMoving = other._isMoving;
	_isJumping = other._isJumping;
	_isAttacking = other._isAttacking;

	_skeletonComp = other._skeletonComp;
	other._skeletonComp = nullptr;

	_sceneComp = std::move(other._sceneComp);
	_animationComp = std::move(other._animationComp);

	return *this;
}

void pa::Character::update(float deltaTime, ID3D11DeviceContext* deviceContext)
{
	using namespace DirectX;
	_animationComp->update(*this, deltaTime);
}

void pa::Character::processInput(float deltaTime)
{
	auto mouse = DirectX::Mouse::Get().GetState();
	auto kb = DirectX::Keyboard::Get().GetState();
	
	if (kb.Space)
	{
		_isJumping = true;
	}

	if (_isJumping)
	{
		_jumpTime = std::min(1.0f, _jumpTime + deltaTime);
	}

	if (kb.F)
	{
		_isAttacking = true;
		_attackTime = std::min(1.0f, _attackTime + deltaTime);
	}
	else
	{
		_isAttacking = false;
		_attackTime = std::max(0.0f, _attackTime - deltaTime);
	}

	if (kb.W)
	{
		_moveTime = std::min(2.0f, _moveTime + deltaTime);
	}
	else
	{
		_moveTime = std::max(0.0f, _moveTime - deltaTime);
	}

	_isMoving = (_moveTime > 0.0f);
}

void pa::Character::setPosition(float x, float y, float z)
{
	_sceneComp->position.x = x;
	_sceneComp->position.y = y;
	_sceneComp->position.z = z;
}
