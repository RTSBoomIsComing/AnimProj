// author: Changwan Yu
#pragma once
#include "IAnimationController.h"
namespace pa
{
	class Animation;
	class AnimationCapture : public IAnimationController
	{
	public:
		AnimationCapture(const Animation* animation, uint32_t frameIndex);

		void update(float deltaTime)	override;
		DirectX::XMVECTOR getBoneRotation(size_t boneIndex, uint32_t offset = 0) const override;

	private:
		const Animation* _animation = nullptr;
	};
}

