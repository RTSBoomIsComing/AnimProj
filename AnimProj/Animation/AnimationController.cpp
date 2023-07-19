#include "pch.h"
#include "AnimationController.h"

pa::AnimationController::AnimationController(const Animation* animation)
	: _animation(animation)
{
	_rotations.resize(animation->getBoneAnimationCount());
	_duration = static_cast<uint32_t>(_animation->getDuration());
	assert(_duration > 0);
}

void pa::AnimationController::update(float deltaTime)
{
	using namespace DirectX;
	constexpr int fps = 120;

	IAnimationController::update(deltaTime);


	static uint16_t trackIndicator;
	uint16_t nControlPoits = _animation->getBoneAnimationCount() * 4 /* P0, P1, P2, P3*/;

	struct ControlPoint
	{
		uint16_t keytime;
		XMFLOAT4 data;
	};

	uint32_t elipsedFrame = static_cast<uint32_t>(_runningTime * fps);
	if (elipsedFrame > _duration)
	{
		_runningTime = 0.0f;
		elipsedFrame = 0;
	}



	for (size_t boneIndex = 0; boneIndex < _animation->getBoneAnimationCount(); boneIndex++)
	{

	}

	//for (size_t boneIndex = 0; boneIndex < _animation->getBoneAnimationCount(); boneIndex++)
	//{
	//	XMVECTOR rotation = playBoneAnimationCatmullRomCyclic(
	//		_animation->_boneAnimation[boneIndex].rotation, elipsedFrame);
	//	_rotations[boneIndex] = rotation;
	//}
}

DirectX::XMVECTOR pa::AnimationController::getBoneRotation(size_t boneIndex, uint32_t offset) const
{
	return _rotations[boneIndex];
}

DirectX::XMVECTOR pa::AnimationController::playBoneAnimationCatmullRomCyclic(std::vector<Animation::Frame> const& frames, uint32_t key, uint32_t offset) const
{
	using namespace DirectX;

	if (frames.empty())
		return { 0.0f, 0.0f, 0.0f, 1.0f };

	assert(offset < frames.size());
	assert(frames.size() >= 2 && "frames need to have two control points, start point and end point");
	uint32_t keyModular = static_cast<uint32_t>(_duration) + 1;
	key = key % keyModular;

	Animation::Frame findFrame{ key };
	auto findIt = std::upper_bound(frames.begin() + offset, frames.end(), findFrame,
		[](Animation::Frame const& f1, Animation::Frame const& f2) {
			return f1.key < f2.key;
		});

	size_t	index2 = findIt != frames.end() ? std::distance(frames.begin(), findIt) : 0;
	size_t	index3 = (index2 + 1) % frames.size();
	size_t	index1 = (index2 > 0) ? index2 - 1 : frames.size() - 1;
	size_t	index0 = (index1 > 0) ? index1 - 1 : frames.size() - 1;

	int 	P0 = frames[index0].key;
	int 	P1 = frames[index1].key;
	int 	P2 = frames[index2].key;
	int 	P3 = frames[index3].key;

	assert(P1 != P2);
	assert(P1 <= static_cast<int>(key));


	float	t = static_cast<float>(key - P1) / (P2 - P1);

	if (P2 < P1)
	{
		t = 0.5f;
	}

	assert(0.0f <= t && t <= 1.0f);

	XMVECTOR quaternion = XMVectorCatmullRom(XMLoadFloat4(&frames[index0].v), XMLoadFloat4(&frames[index1].v),
		XMLoadFloat4(&frames[index2].v), XMLoadFloat4(&frames[index3].v), t);

	return quaternion;
}