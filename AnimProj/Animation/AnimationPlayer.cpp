// author: Changwan Yu
#include "pch.h"
#include "AnimationPlayer.h"
#include "Animation.h"

pa::AnimationPlayer::AnimationPlayer(const Animation& animation)
	: _animation(animation)
	, _duration(_animation.getKeyframes().back().keytime)
{
	_activeKeys.resize(_animation.getTrackHeaders().size());
	initializeActiveKeysWithMemCopy();

	_isRunning = false;
}

void pa::AnimationPlayer::update(float deltaTime)
{
	using namespace DirectX;
	constexpr int fps = 120;

	if (!_isRunning)
		return;

	_runningTime += deltaTime;
	_runningFrame = _runningTime * fps;

	if (_isLooping && _duration < _runningFrame)
	{
		_runningTime = 0;
		_runningFrame = 0;
		initializeActiveKeysWithMemCopy();
	}

	if (_duration < _runningFrame)
	{
		_isRunning = false;
		_runningFrame = _duration;
	}

	while (_cursor < _animation.getKeyframes().size())
	{
		uint16_t trackID = _animation.getTrackIDs()[_cursor];
		auto& controlPoints = _activeKeys[trackID];
		if (controlPoints[2].keytime < _runningFrame)
		{
			controlPoints[0] = controlPoints[1];
			controlPoints[1] = controlPoints[2];
			controlPoints[2] = controlPoints[3];
			controlPoints[3] = _animation.getKeyframes()[_cursor];

			_cursor++;
		}
		else
			break;
	}
}

void pa::AnimationPlayer::cachePose(std::vector<Transform>& outPose) const
{
	cachePoseWithBlending(outPose, 1.0f);
}

void pa::AnimationPlayer::cachePoseWithBlending(std::vector<Transform>& basePose, float weight) const
{
	using namespace DirectX;

	weight = std::max(weight, 0.0f);
	weight = std::min(weight, 1.0f);
	if (0.0f == weight)
		return;

	//cache rotation
	for (int trackID = 0; trackID < _activeKeys.size(); trackID++)
	{
		const uint16_t boneID = _animation.getTrackHeaders()[trackID].boneID;
		const auto& controlPoints = _activeKeys[trackID];

		if (controlPoints[1].keytime >= controlPoints[2].keytime)
			DebugBreak();

		if (controlPoints[1].keytime > _runningFrame || _runningFrame > controlPoints[2].keytime)
			DebugBreak();

		float t = (_runningFrame - controlPoints[1].keytime)
			/ (controlPoints[2].keytime - controlPoints[1].keytime);

		t = std::min(1.0f, t);

		if (AnimationTrackType::Rotation == _animation.getTrackHeaders()[trackID].type)
		{
			XMVECTOR Q1 = XMQuaternionSlerp(
				controlPoints[1].decompressAsQuaternion(),
				controlPoints[2].decompressAsQuaternion(), t);

			if (1.0f == weight)
			{
				XMStoreFloat4(&basePose[boneID].rotation, Q1);
				continue;
			}

			XMVECTOR Q0 = XMLoadFloat4(&basePose[boneID].rotation);
			XMVECTOR Qresult = XMQuaternionSlerp(Q0, Q1, weight);
			XMStoreFloat4(&basePose[boneID].rotation, Qresult);

			//_rotations[_animation->_trackDescriptors[i]] = cp[1].decompressAsQuaternion();

			//_rotations[_animation->_trackDescriptors[i]] = 
			//	XMVectorLerp(cp[1].decompressAsQuaternion(), cp[2].decompressAsQuaternion(), weight);

			//_rotations[boneID] = XMQuaternionNormalize(XMVectorCatmullRom(
			//	controlPoints[0].decompressAsQuaternion(),
			//	controlPoints[1].decompressAsQuaternion(),
			//	controlPoints[2].decompressAsQuaternion(),
			//	controlPoints[3].decompressAsQuaternion(), weight));
		}
	}
}

void pa::AnimationPlayer::play()
{
	_isRunning = true;
}

void pa::AnimationPlayer::stop()
{
	_isRunning = false;
}

void pa::AnimationPlayer::reset()
{
	if (0 == _runningTime)
		return;

	_runningTime = 0.0f;
	_runningFrame = 0.0f;
	initializeActiveKeysWithMemCopy();
}

void pa::AnimationPlayer::setLoop(bool enable)
{
	_isLooping = enable;
}

void pa::AnimationPlayer::initializeActiveKeys()
{
	const size_t trackCount = _animation.getTrackHeaders().size();
	_cursor = static_cast<uint32_t>(trackCount) * 4;
	for (size_t i = 0; i < _cursor; i++)
	{
		const uint16_t trackID = _animation.getTrackIDs()[i];

		_activeKeys[trackID][0] = _activeKeys[trackID][1];
		_activeKeys[trackID][1] = _activeKeys[trackID][2];
		_activeKeys[trackID][2] = _activeKeys[trackID][3];
		_activeKeys[trackID][3] = _animation.getKeyframes()[i];
	}
}

void pa::AnimationPlayer::initializeActiveKeysWithMemCopy()
{
	const size_t trackCount = _animation.getTrackHeaders().size();
	_cursor = static_cast<uint32_t>(trackCount) * 4;
	std::memcpy(_activeKeys.data(), _animation.getKeyframes().data(), sizeof(CompactKey) * _cursor);
}
