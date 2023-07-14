#include "pch.h"
#include "AnimationController.h"
#include "Animation.h"

pa::AnimationController::AnimationController(const Animation* animation)
	: _animation(animation)
{
	_rotations.resize(animation->getBoneAnimationCount());
	_duration = _animation->getDuration();
	assert(_duration > 0);
}

void pa::AnimationController::update(float deltaTime)
{
	using namespace DirectX;
	constexpr int fps = 120;

	IAnimationController::update(deltaTime);

	uint32_t elipsedFrame = static_cast<uint32_t>(_runningTime * fps);
	if (elipsedFrame > _duration)
	{
		_runningTime = 0.0f;
		elipsedFrame = 0;
	}

	for (size_t boneIndex = 0; boneIndex < _animation->getBoneAnimationCount(); boneIndex++)
	{
		XMVECTOR rotation = _animation->getBoneRotation(boneIndex, elipsedFrame);
		_rotations[boneIndex] = rotation;
	}
}

DirectX::XMVECTOR pa::AnimationController::getBoneRotation(size_t boneIndex, uint32_t offset) const
{
	return _rotations[boneIndex];
}
