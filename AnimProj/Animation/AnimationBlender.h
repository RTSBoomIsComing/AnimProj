#pragma once
#include "Animation.h"
namespace pa
{
	class AnimationBlender
	{
	public:
		AnimationBlender() = default;
		~AnimationBlender() = default;

	public:
		DirectX::XMMATRIX blendAnimationWeight(Animation* basePose, Animation* animation1, uint32_t key, float weight);
	private:

	};
}