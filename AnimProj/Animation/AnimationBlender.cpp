#include "pch.h"
#include "AnimationBlender.h"
#include "Animation.h"

pa::AnimationBlender::AnimationBlender(const Animation* base, const Animation* blend)
	: _baseAnimation(base)
	, _blendAnimation(blend)
{
	_rotations.resize(base->getBoneAnimationCount());

	_blendSync = static_cast<float>(blend->getDuration()) / base->getDuration();

}

void pa::AnimationBlender::update(float deltaTime)
{
	using namespace DirectX;
	constexpr int fps = 120;
	if (false == isRunning())
		return;

	_runningTime += deltaTime;
	_playSpeed = 1.0f * (1 - _blendWeight) + 1 / _blendSync * _blendWeight;

	uint32_t baseElipsedFrame = static_cast<uint32_t>(_runningTime * _playSpeed * fps);
	uint32_t blendElipsedFrame = static_cast<uint32_t>(_runningTime * _playSpeed * _blendSync * fps);
	if (_baseAnimation->getDuration() < baseElipsedFrame)
	{
		_runningTime = 0.0f;
		baseElipsedFrame = 0;
	}

	for (size_t boneIndex = 0; boneIndex < _baseAnimation->getBoneAnimationCount(); boneIndex++)
	{
		XMVECTOR baseRotation = XMQuaternionNormalize(_baseAnimation->getBoneRotation(boneIndex, baseElipsedFrame));
		XMVECTOR blendRotation = XMQuaternionNormalize(_blendAnimation->getBoneRotation(boneIndex, blendElipsedFrame));
		_rotations[boneIndex] = XMQuaternionNormalize(
			XMQuaternionSlerp(baseRotation, blendRotation, _blendWeight));
	}
}

void pa::AnimationBlender::addBlendWeight(float weight)
{
	_blendWeight += weight;
	if (_blendWeight > 1.0f)
		_blendWeight = 1.0f;

	else if (_blendWeight < 0.0f)
		_blendWeight = 0.0f;
}

void pa::AnimationBlender::addPlaySpeed(float speed)
{
	_playSpeed += speed;
	if (_playSpeed > 1.0f)
		_playSpeed = 1.0f;

	else if (_playSpeed < 0.0f)
		_playSpeed = 0.0f;
}
