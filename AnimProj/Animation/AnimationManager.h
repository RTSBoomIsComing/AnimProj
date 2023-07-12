// author: Changwan Yu
#pragma once
#include "Animation.h"
#include "AnimationBlender.h"

namespace pa
{
	class Skeleton;
	class Animation;
	class AnimationManager
	{
	public:
		AnimationManager();
		~AnimationManager();

		void initialize();

	private:
		Skeleton*						_skeleton = nullptr;
		std::vector<Animation>			_animations;
		std::vector<AnimationBlender>	_animationBlenders;

	};
}

