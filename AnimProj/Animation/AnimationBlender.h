// author: Changwan Yu
#pragma once
#include "IAnimationController.h"
namespace pa
{
	class AnimationController;
	class AnimationBlender : public IAnimationController
	{
	public:
		AnimationBlender(const AnimationController* base, const AnimationController* blend);
		~AnimationBlender() = default;

		void update(float deltaTime)override;

		DirectX::XMVECTOR getBoneRotation(size_t boneIndex, uint32_t offset = 0) const override;

	private:
		const AnimationController*	_baseAnimation	= nullptr;
		const AnimationController*	_blendAnimation	= nullptr;
		float						_blendSync		= 1.0f;
	};
}

