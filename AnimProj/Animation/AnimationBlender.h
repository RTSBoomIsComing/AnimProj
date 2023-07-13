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

		void update(float deltaTime)override;
		void addBlendWeight(float weight);

		DirectX::XMVECTOR getBoneRotation(size_t boneIndex, uint32_t offset = 0) const override;

	private:
		const Animation*	_baseAnimation	= nullptr;
		const Animation*	_blendAnimation	= nullptr;

		float				_blendWeight	= 0.5f;
		float				_blendSync		= 1.0f;
	};
}

