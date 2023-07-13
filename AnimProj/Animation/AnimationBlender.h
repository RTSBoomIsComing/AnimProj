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
		void addBlendWeight(float weight);
		void addPlaySpeed(float speed);
	private:
		const Animation*	_baseAnimation	= nullptr;
		const Animation*	_blendAnimation	= nullptr;

		float				_blendWeight	= 0.5f;
		float				_playSpeed		= 1.0f;
		float				_blendSync		= 1.0f;
	};
}

