// author: Changwan Yu
#pragma once
#include "IAnimationController.h"
//#include "KeyFrame.h"
namespace pa
{
	class Animation;
	struct CatmullRomCyclicPlayer
	{

	};
	class AnimationController : public IAnimationController
	{

	public:
		AnimationController(const Animation* animation);
		~AnimationController() = default;

		void update(float deltaTime)	override;
		void reset()					override;

	private:
		const Animation*				_animation		= nullptr;
		float							_playSpeed		= 1.0f;
	};

}

