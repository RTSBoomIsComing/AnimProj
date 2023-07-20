#include "pch.h"
#include "AnimationController.h"
#include "CompactKeyframe.h"

pa::AnimationController::AnimationController(const Animation* animation)
	: _animation(animation)
{
	_rotations.resize(animation->getBoneCount(), {0.0f, 0.0f, 0.0f, 1.0f});
	_duration = static_cast<uint32_t>(_animation->getDuration());
	assert(_duration > 0);
}

void pa::AnimationController::update(float deltaTime)
{
	using namespace DirectX;
	constexpr int fps = 120;

	IAnimationController::update(deltaTime);


	static std::vector<std::array<CompactKeyframe, 4>> active(_animation->_rotationTrackDescriptors.size());
	static uint32_t cursor = 0;
	static uint32_t timeCursor = 0;
	float elipsedFrame = _runningTime * fps;
	//static float elipsedFrame = 0.0f;
	//elipsedFrame += 0.1f;
	static bool startup = true;
	if (startup || _isCyclic && _duration < elipsedFrame)
	{
		startup = false;
		elipsedFrame = 0;
		_runningTime = 0;
		cursor = 0;
		timeCursor = 0;
	}

	if (0 == cursor)
	{
		cursor = _animation->_rotationTrackDescriptors.size() * 4;
		std::memcpy(active.data(), _animation->_compactStream.data(), sizeof(CompactKeyframe) * cursor);
	}

	for (; timeCursor <= 1 + static_cast<uint32_t>(elipsedFrame); timeCursor++)
	{
		for (auto& cp : active)
		{
			if (cursor == _animation->_compactStream.size())
				break;

			if (static_cast<float>(cp[2].keytime) < timeCursor)
			{
				cp[0] = cp[1];
				cp[1] = cp[2];
				cp[2] = cp[3];
				cp[3] = _animation->_compactStream[cursor++];
			}
		}
	}
	
	for (int i = 0; i < active.size(); i++)
	{
		const auto& cp = active[i];
		if (!(cp[1].keytime < cp[2].keytime))
			DebugBreak();
		if (!(cp[1].keytime <= elipsedFrame && elipsedFrame <= cp[2].keytime))
			DebugBreak();

		float weight = (elipsedFrame - cp[1].keytime) / (cp[2].keytime - cp[1].keytime);

		//_rotations[_animation->_trackDescriptors[i]] = cp[1].decompressAsQuaternion();

		_rotations[_animation->_rotationTrackDescriptors[i].id] =
			XMQuaternionSlerp(cp[1].decompressAsQuaternion(), cp[2].decompressAsQuaternion(), weight);
		
		//_rotations[_animation->_trackDescriptors[i]] = 
		//	XMVectorLerp(cp[1].decompressAsQuaternion(), cp[2].decompressAsQuaternion(), weight);

		//_rotations[_animation->_trackDescriptors[i]] = XMQuaternionNormalize(XMVectorCatmullRom(
		//	cp[0].decompressAsQuaternion(),
		//	cp[1].decompressAsQuaternion(),
		//	cp[2].decompressAsQuaternion(),
		//	cp[3].decompressAsQuaternion(), weight));
	}

	// regacy, catmullrom with binary search
	//for (size_t boneIndex = 0; boneIndex < _animation->getBoneCount(); boneIndex++)
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

DirectX::XMVECTOR pa::AnimationController::playBoneAnimationCatmullRomCyclic(std::vector<Animation::Keyframe> const& frames, uint32_t key, uint32_t offset) const
{
	using namespace DirectX;

	if (frames.empty())
		return { 0.0f, 0.0f, 0.0f, 1.0f };

	assert(offset < frames.size());
	assert(frames.size() >= 2 && "frames need to have two control points, start point and end point");
	uint32_t keyModular = static_cast<uint32_t>(_duration) + 1;
	key = key % keyModular;

	Animation::Keyframe findFrame{ key };
	auto findIt = std::upper_bound(frames.begin() + offset, frames.end(), findFrame,
		[](Animation::Keyframe const& f1, Animation::Keyframe const& f2) {
			return f1.keytime < f2.keytime;
		});

	size_t	index2 = findIt != frames.end() ? std::distance(frames.begin(), findIt) : 0;
	size_t	index3 = (index2 + 1) % frames.size();
	size_t	index1 = (index2 > 0) ? index2 - 1 : frames.size() - 1;
	size_t	index0 = (index1 > 0) ? index1 - 1 : frames.size() - 1;

	int 	P0 = frames[index0].keytime;
	int 	P1 = frames[index1].keytime;
	int 	P2 = frames[index2].keytime;
	int 	P3 = frames[index3].keytime;

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
