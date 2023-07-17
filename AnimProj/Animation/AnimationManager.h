// author: Changwan Yu
#pragma once
#include "Animation.h"
#include "IAnimationController.h"
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
		
		static AnimationManager* get()
		{
			static AnimationManager sAnimationManager;
			return &sAnimationManager;
		}

		void initialize();
		//void loadAnimations();
		//void loadAnimationControllers();
		//void loadAnimationBlenders();

	private:
		Skeleton*							_skeleton					= nullptr;
		std::vector<Animation>				_animationList;
		std::vector<IAnimationController*>	_animationControllerList;

	};
}

//extern pa::AnimationManager* GAnimationManager;


