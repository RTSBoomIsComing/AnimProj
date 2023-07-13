#include "pch.h"
#include "AnimationBlender.h"
#include "Animation.h"

pa::AnimationBlender::AnimationBlender(const Animation* base, const Animation* blend)
	: _baseAnimation(base)
	, _blendAnimation(blend)
{
	_rotations.resize(base->getBoneAnimationCount());

}

void pa::AnimationBlender::update(float deltaTime)
{
	using namespace DirectX;
	constexpr int fps = 120;
	if (false == isRunning())
		return;

	_runningTime += deltaTime;

	uint32_t elipsedFrame = static_cast<uint32_t>(deltaTime * fps);
	if (_baseAnimation->getDuration() < elipsedFrame)
	{
		_runningTime = 0.0f;
		elipsedFrame = 0;
	}

	for (size_t boneIndex = 0; boneIndex < _baseAnimation->getBoneAnimationCount(); boneIndex++)
	{
		XMVECTOR rotation = _baseAnimation->getBoneRotation(boneIndex, elipsedFrame);
		_rotations[boneIndex] = rotation;
	}


}

void pa::AnimationBlender::reset()
{
}
