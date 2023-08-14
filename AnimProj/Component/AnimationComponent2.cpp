// author: Changwan Yu
#include "pch.h"
#include "AnimationComponent2.h"
#include "../Animation/Animation.h"

namespace pa
{
	void AnimationComponent2::onUpdate(World& world, Actor& owner, float deltaTime)
	{
	}
	void AnimationComponent2::transitAnimationUpperBody(const Animation& animation, float transitionTime)
	{
		_upperBody.activeKeys.resize(animation.getTrackHeaders().size());
		initializeActiveKeysWithMemCopy();
	}
	void AnimationComponent2::transitAnimationLowerBody(const Animation& animation, float transitionTime)
	{
		_lowerBody.activeKeys.resize(animation.getTrackHeaders().size());
		
	}
}
