// author: Changwan Yu
#include "pch.h"
#include "AnimationComponent.h"
//#include "../Rendering/Character.h"
#include "../World/World.h"
#include "../Actor/Actor.h"
#include "../Animation/Skeleton.h"
#include "../Animation/AnimationManager.h"

namespace pa
{
	AnimationComponent::AnimationComponent(const Skeleton& skeleton)
		: _skeleton(&skeleton)
	{
		const size_t boneCount = skeleton.getBoneCount();
		_resultPose.resize(boneCount);

		for (const auto& anim : AnimationManager::get().getAnimationList())
		{
			_animationPlayers.push_back(AnimationPlayer(anim));
		}

		this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).setLoop(false);
		this->getAnimationPlayer(AnimPlayerIndex::Jump_up).setLoop(false);
	}

	//void AnimationComponent::onUpdate(World& world, Actor& owner, float deltaTime)
	//{
	//	for (auto& animationPlayer : _animationPlayers)
	//	{
	//		animationPlayer.update(deltaTime);
	//	}

	//	if (this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).getRunningRate() >= 1.0f)
	//	{
	//		owner._isJumping  = false;
	//		owner._jumpTime	  = 0.0f;
	//		owner._moveTime	  = 0.0f;
	//		owner._attackTime = 0.0f;
	//		owner._moveTime	  = 0.0f;
	//		this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).reset();
	//		this->getAnimationPlayer(AnimPlayerIndex::Jump_up).reset();
	//	}

	//	if (owner._isJumping)
	//	{
	//		this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).play();
	//		this->getAnimationPlayer(AnimPlayerIndex::Jump_up).play();
	//	}

	//	if (owner._isMoving)
	//	{
	//		this->getAnimationPlayer(AnimPlayerIndex::Walk_up).play();
	//		this->getAnimationPlayer(AnimPlayerIndex::Walk_lo).play();
	//		this->getAnimationPlayer(AnimPlayerIndex::Run_up).play();
	//		this->getAnimationPlayer(AnimPlayerIndex::Run_lo).play();
	//	}
	//	else
	//	{
	//		this->getAnimationPlayer(AnimPlayerIndex::Walk_up).stop();
	//		this->getAnimationPlayer(AnimPlayerIndex::Walk_lo).stop();
	//		this->getAnimationPlayer(AnimPlayerIndex::Run_up).stop();
	//		this->getAnimationPlayer(AnimPlayerIndex::Run_lo).stop();
	//	}

	//	if (owner._isAttacking)
	//	{
	//		this->getAnimationPlayer(AnimPlayerIndex::Punch_up).play();
	//		this->getAnimationPlayer(AnimPlayerIndex::Punch_lo).play();
	//	}
	//	else
	//	{
	//		this->getAnimationPlayer(AnimPlayerIndex::Punch_up).stop();
	//		this->getAnimationPlayer(AnimPlayerIndex::Punch_lo).stop();
	//	}

	//	if (!owner._isJumping)
	//	{
	//		this->getAnimationPlayer(AnimPlayerIndex::Jump_up).cachePose(_resultPose);
	//		this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).cachePose(_resultPose);
	//	}

	//	this->getAnimationPlayer(AnimPlayerIndex::Walk_up).cachePoseWithBlending(_resultPose, owner._moveTime);
	//	this->getAnimationPlayer(AnimPlayerIndex::Walk_lo).cachePoseWithBlending(_resultPose, owner._moveTime);

	//	this->getAnimationPlayer(AnimPlayerIndex::Run_up).cachePoseWithBlending(_resultPose, owner._moveTime - 1.0f);
	//	this->getAnimationPlayer(AnimPlayerIndex::Run_lo).cachePoseWithBlending(_resultPose, owner._moveTime - 1.0f);

	//	this->getAnimationPlayer(AnimPlayerIndex::Punch_up).cachePoseWithBlending(_resultPose, owner._attackTime);

	//	this->getAnimationPlayer(AnimPlayerIndex::Jump_up).cachePoseWithBlending(_resultPose, owner._jumpTime);
	//	this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).cachePoseWithBlending(_resultPose, owner._jumpTime);
	//}

	void AnimationComponent::setSkeleton(const Skeleton& skeleton)
	{
		_skeleton = &skeleton;
		const size_t boneCount = skeleton.getBoneCount();
		_resultPose.resize(boneCount);

		for (const auto& anim : AnimationManager::get().getAnimationList())
		{
			_animationPlayers.push_back(AnimationPlayer(anim));
		}

		this->getAnimationPlayer(AnimPlayerIndex::Jump_lo).setLoop(false);
		this->getAnimationPlayer(AnimPlayerIndex::Jump_up).setLoop(false);
	}


	AnimationPlayer& AnimationComponent::getAnimationPlayer(AnimPlayerIndex index)
	{
		return _animationPlayers[static_cast<size_t>(index)];
	}
}