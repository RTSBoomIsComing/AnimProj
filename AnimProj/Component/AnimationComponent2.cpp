// author: Changwan Yu
#include "pch.h"
#include "AnimationComponent2.h"
#include "../Animation/Animation.h"
#include "../World/World.h"
#include "../Actor/Actor.h"
#include "../Component/SkeletalMeshComponent.h"

namespace pa
{
	void AnimationComponent2::onUpdate(World& world, Actor& owner, float deltaTime)
	{
		_upperBody.onUpdate(deltaTime);
		_lowerBody.onUpdate(deltaTime);

		// Blend animation layer
		// TODO: Make this logic as function
		SkeletalMeshComponent* skeletalMeshComp = owner.getComponent<SkeletalMeshComponent>();

		if (_lowerBody.animation)
		{
			for (int trackID = 0; trackID < _lowerBody.activeKeys.size(); trackID++)
			{
				const uint16_t boneID = _lowerBody.animation->getTrackHeaders()[trackID].boneID;
				if (AnimationTrackType::Rotation == _lowerBody.animation->getTrackHeaders()[trackID].type)
				{
					skeletalMeshComp->_pose[boneID] = _lowerBody.currentPose[boneID];
				}
			}
		}

		if (_upperBody.animation)
		{
			for (int trackID = 0; trackID < _upperBody.activeKeys.size(); trackID++)
			{
				const uint16_t boneID = _upperBody.animation->getTrackHeaders()[trackID].boneID;
				if (AnimationTrackType::Rotation == _upperBody.animation->getTrackHeaders()[trackID].type)
				{
					skeletalMeshComp->_pose[boneID] = _upperBody.currentPose[boneID];
				}
			}
		}
	}

	void AnimationComponent2::transitAnimationUpperBody(const Animation& animation, float transitionTime)
	{
		_upperBody.transitAnimation(animation, transitionTime);
	}

	void AnimationComponent2::transitAnimationLowerBody(const Animation& animation, float transitionTime)
	{
		_lowerBody.transitAnimation(animation, transitionTime);
	}

	void AnimationComponent2::Layer::transitAnimation(const Animation& animation, float transitionTime)
	{
		this->animation		 = &animation;
		this->transitionTime = transitionTime;
		this->playTime		 = 0.0f;

		activeKeys.resize(this->animation->getTrackHeaders().size());
		lastPose.swap(currentPose);
		currentPose.clear();
		currentPose.resize(lastPose.size());

		const size_t trackCount = this->animation->getTrackHeaders().size();
		cursor					= trackCount * 4;
		std::memcpy(activeKeys.data(), this->animation->getKeyframes().data(), sizeof(CompactKey) * cursor);
	}

	void AnimationComponent2::Layer::onUpdate(float deltaTime)
	{
		using namespace DirectX;
		constexpr int fps = 120;

		if (nullptr == animation)
		{
			// TODO: Notify End animation
			return;
		}

		playTime += deltaTime;
		float runningFrame = playTime * fps;

		const uint16_t duration = animation->getKeyframes().back().keytime;
		if (duration < runningFrame)
		{
			animation = nullptr;
			//runningFrame = duration;
			return;
		}

		while (cursor < animation->getKeyframes().size())
		{
			uint16_t trackID	   = animation->getTrackIDs()[cursor];
			auto&	 controlPoints = activeKeys[trackID];
			if (controlPoints[2].keytime < runningFrame)
			{
				controlPoints[0] = controlPoints[1];
				controlPoints[1] = controlPoints[2];
				controlPoints[2] = controlPoints[3];
				controlPoints[3] = animation->getKeyframes()[cursor];

				cursor++;
			}
			else
				break;
		}

		//cache pose
		float transitionBlendWeight = playTime / transitionTime;
		transitionBlendWeight		= std::min(transitionBlendWeight, 1.0f);

		for (int trackID = 0; trackID < activeKeys.size(); trackID++)
		{
			const uint16_t boneID		 = animation->getTrackHeaders()[trackID].boneID;
			const auto&	   controlPoints = activeKeys[trackID];

			if (controlPoints[1].keytime >= controlPoints[2].keytime)
				DebugBreak();

			if (controlPoints[1].keytime > runningFrame || runningFrame > controlPoints[2].keytime)
				DebugBreak();

			float t = (runningFrame - controlPoints[1].keytime) / (controlPoints[2].keytime - controlPoints[1].keytime);

			t = std::min(1.0f, t);

			if (AnimationTrackType::Rotation == animation->getTrackHeaders()[trackID].type)
			{
				XMVECTOR Q1 = XMQuaternionSlerp(
					controlPoints[1].decompressAsQuaternion(),
					controlPoints[2].decompressAsQuaternion(), t);

				if (1.0f == transitionBlendWeight)
				{
					XMStoreFloat4(&currentPose[boneID].rotation, Q1);
				}
				else
				{
					XMVECTOR Q0		 = XMLoadFloat4(&lastPose[boneID].rotation);
					XMVECTOR Qresult = XMQuaternionSlerp(Q0, Q1, transitionBlendWeight);
					XMStoreFloat4(&currentPose[boneID].rotation, Qresult);
				}

				//_rotations[_animation->_trackDescriptors[i]] = cp[1].decompressAsQuaternion();

				//_rotations[_animation->_trackDescriptors[i]] =
				//	XMVectorLerp(cp[1].decompressAsQuaternion(), cp[2].decompressAsQuaternion(), transitionBlendWeight);

				//_rotations[boneID] = XMQuaternionNormalize(XMVectorCatmullRom(
				//	controlPoints[0].decompressAsQuaternion(),
				//	controlPoints[1].decompressAsQuaternion(),
				//	controlPoints[2].decompressAsQuaternion(),
				//	controlPoints[3].decompressAsQuaternion(), transitionBlendWeight));
			}
		}
	}
}
