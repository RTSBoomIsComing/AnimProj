// author: Changwan Yu
#include "pch.h"
#include "AnimationCapture.h"
#include "Animation.h"

pa::AnimationCapture::AnimationCapture(const Animation* animation, uint32_t frameIndex)
	: _animation(animation)
{
	_duration = 0;

	_rotations.reserve(_animation->getBoneAnimationCount());
	for (size_t boneIndex = 0; boneIndex < _animation->getBoneAnimationCount(); boneIndex++)
	{
		_rotations.push_back(_animation->getBoneRotation(boneIndex));
	}
}

void pa::AnimationCapture::update(float deltaTime)
{
	IAnimationController::update(deltaTime);
}

DirectX::XMVECTOR pa::AnimationCapture::getBoneRotation(size_t boneIndex, uint32_t offset) const
{
	return _rotations[boneIndex];
}
