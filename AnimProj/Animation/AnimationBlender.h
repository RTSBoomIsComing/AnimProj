// author: Changwan Yu
#pragma once
#include "IAnimationController.h"
namespace pa
{
	class Animation;
	class AnimationBlender : public IAnimationController
	{
	public:
		AnimationBlender(const Animation* base, const Animation* blend);
		~AnimationBlender() = default;

		void update(float deltaTime)	override;
		void reset()					override;

	private:
		const Animation* _baseAnimation;
		const Animation* _blendAnimation;
	};
}

