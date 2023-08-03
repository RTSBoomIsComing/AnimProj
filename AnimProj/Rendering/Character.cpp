#include "pch.h"
#include "Character.h"
#include "../Animation/Skeleton.h"
#include "../Component/SkeletonComponent.h"
#include "../Component/AnimationComponent.h"
#include "../Animation/AnimationManager.h"

#include <DirectXTK/Keyboard.h>
#include <DirectXTK/Mouse.h>

pa::Character::Character(ID3D11Device* device)
{
	_skeleton = &AnimationManager::get().getDefaultSkeleton();
	_skeletonComp = std::make_shared<SkeletonComponent>(device, *_skeleton);
	_animationComp = std::make_shared<AnimationComponent>(device);

	_poseCache[0].resize(_skeleton->getBoneCount());
	_poseCache[1].resize(_skeleton->getBoneCount());

	for (const auto& anim : AnimationManager::get().getAnimationList())
	{
		_animationPlayers.push_back(AnimationPlayer(anim));
	}

	this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).setLoop(false);
	this->getAnimationPlayer(AnimPlayerIndex::Jump_up).setLoop(false);
}

pa::Character::~Character() = default;

void pa::Character::update(float deltaTime, ID3D11DeviceContext* deviceContext)
{
	using namespace DirectX;

	for (auto& animationPlayer : _animationPlayers)
	{
		animationPlayer.update(deltaTime);
	}

	this->updatePose();
	_skeletonComp->update(deviceContext, _poseCache[0].data(), _worldPosition, _worldRotation);
}

void pa::Character::processInput(float deltaTime)
{
	auto mouse = DirectX::Mouse::Get().GetState();
	if (mouse.leftButton)
	{
		std::cout << mouse.x << " " << mouse.y << std::endl;
	}

	auto kb = DirectX::Keyboard::Get().GetState();
	if (kb.Back)
	{
		for (auto& animationPlayer : _animationPlayers)
		{
			animationPlayer.stop();
		}
	}

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

pa::AnimationPlayer& pa::Character::getAnimationPlayer(AnimPlayerIndex index)
{
	return _animationPlayers[static_cast<size_t>(index)];
}

void pa::Character::updatePose()
{
	using namespace DirectX;

	if (this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).getRunningRate() >= 1.0f)
	{
		_isJumping = false;
		_jumpTime = 0.0f;
		_moveTime = 0.0f;
		_attackTime = 0.0f;
		_moveTime = 0.0f;
		this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).reset();
		this->getAnimationPlayer(AnimPlayerIndex::Jump_up).reset();
	}

	if (_isJumping)
	{
		this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).play();
		this->getAnimationPlayer(AnimPlayerIndex::Jump_up).play();
	}

	if (_isMoving)
	{
		this->getAnimationPlayer(AnimPlayerIndex::Walk_up).play();
		this->getAnimationPlayer(AnimPlayerIndex::Walk_lo).play();
		this->getAnimationPlayer(AnimPlayerIndex::Run_up).play();
		this->getAnimationPlayer(AnimPlayerIndex::Run_lo).play();
	}
	else
	{
		this->getAnimationPlayer(AnimPlayerIndex::Walk_up).stop();
		this->getAnimationPlayer(AnimPlayerIndex::Walk_lo).stop();
		this->getAnimationPlayer(AnimPlayerIndex::Run_up).stop();
		this->getAnimationPlayer(AnimPlayerIndex::Run_lo).stop();
	}
	
	if (_isAttacking)
	{
		this->getAnimationPlayer(AnimPlayerIndex::Punch_up).play();
		this->getAnimationPlayer(AnimPlayerIndex::Punch_lo).play();
	}
	else
	{
		this->getAnimationPlayer(AnimPlayerIndex::Punch_up).stop();
		this->getAnimationPlayer(AnimPlayerIndex::Punch_lo).stop();
	}

	if (!_isJumping)
	{
		this->getAnimationPlayer(AnimPlayerIndex::Jump_up).cachePose(_poseCache[0]);
		this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).cachePose(_poseCache[0]);
	}

	this->getAnimationPlayer(AnimPlayerIndex::Walk_up).cachePoseWithBlending(_poseCache[0], _moveTime);
	this->getAnimationPlayer(AnimPlayerIndex::Walk_lo).cachePoseWithBlending(_poseCache[0], _moveTime);

	this->getAnimationPlayer(AnimPlayerIndex::Run_up).cachePoseWithBlending(_poseCache[0], _moveTime - 1.0f);
	this->getAnimationPlayer(AnimPlayerIndex::Run_lo).cachePoseWithBlending(_poseCache[0], _moveTime - 1.0f);
	
	this->getAnimationPlayer(AnimPlayerIndex::Punch_up).cachePoseWithBlending(_poseCache[0], _attackTime);
	
	this->getAnimationPlayer(AnimPlayerIndex::Jump_up).cachePoseWithBlending(_poseCache[0], _jumpTime);
	this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).cachePoseWithBlending(_poseCache[0], _jumpTime);

}
