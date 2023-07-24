// author: Changwan Yu
#pragma once

namespace pa
{
	class Skeleton;
	class CompactAnimation;
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
		//std::vector<CompactAnimation>		_animationList;
	};
}

//extern pa::AnimationManager* GAnimationManager;


