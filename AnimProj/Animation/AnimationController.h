// author: Changwan Yu
#pragma once
#include "IAnimationController.h"
#include "Animation.h"
namespace pa
{
	class Animation;
	class AnimationController : public IAnimationController
	{

	public:
		AnimationController(const Animation* animation);
		~AnimationController() = default;

		void update(float deltaTime)	override;
		DirectX::XMVECTOR getBoneRotation(size_t boneIndex, uint32_t offset = 0) const override;

	private:
		DirectX::XMVECTOR	playBoneAnimationCatmullRomCyclic(std::vector<Animation::Frame> const& frames, uint32_t key, uint32_t offset = 0) const;

	private:
		const Animation*				_animation		= nullptr;
	};

}

