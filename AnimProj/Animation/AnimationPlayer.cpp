// author: Changwan Yu
#include "pch.h"
#include "AnimationPlayer.h"
#include "CompactAnimation.h"

pa::AnimationPlayer::AnimationPlayer(const CompactAnimation& animation)
	: _animation(animation)
	, _duration(_animation.getKeyframes().back().keytime)
{
	_activeKeys.resize(_animation.getTrackHeaders().size());
	initializeActiveKeysWithMemCopy();

	_rotations.resize(41);
	_isRunning = false;
}

void pa::AnimationPlayer::update(float deltaTime)
{
	using namespace DirectX;
	constexpr int fps = 120;

	if (!_isRunning)
		return;
		
	_runningTime += deltaTime;
	
	float elipsedFrame = _runningTime * fps;
	if (_isLooping && _duration < elipsedFrame)
	{
		elipsedFrame = 0;
		_runningTime = 0;
		initializeActiveKeysWithMemCopy();
	}

	while (_cursor < _animation.getKeyframes().size())
	{
		uint16_t trackID = _animation.getTrackIDs()[_cursor];
		auto& controlPoints = _activeKeys[trackID];
		if (controlPoints[2].keytime < elipsedFrame)
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

	//cache rotation
	for (int trackID = 0; trackID < _activeKeys.size(); trackID++)
	{
		uint16_t boneID = _animation.getTrackHeaders()[trackID].boneID;
		auto& controlPoints = _activeKeys[trackID];


		if (!(controlPoints[1].keytime < controlPoints[2].keytime))
			DebugBreak();
		if (!(controlPoints[1].keytime <= elipsedFrame && elipsedFrame <= controlPoints[2].keytime))
			DebugBreak();

		float weight = (elipsedFrame - controlPoints[1].keytime)
			/ (controlPoints[2].keytime - controlPoints[1].keytime);

		if (AnimationType::Rotation == _animation.getTrackHeaders()[trackID].type)
		{
			_rotations[boneID] = XMQuaternionSlerp(
				controlPoints[1].decompressAsQuaternion(),
				controlPoints[2].decompressAsQuaternion(), weight);

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

DirectX::XMVECTOR pa::AnimationPlayer::getBoneRotation(uint32_t boneIndex) const
{
	return _rotations[boneIndex];
}

void pa::AnimationPlayer::play()
{
	_isRunning = true;
}

void pa::AnimationPlayer::initializeActiveKeys()
{
	const size_t trackCount = _animation.getTrackHeaders().size();
	_cursor = trackCount * 4;
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
	_cursor = trackCount * 4;
	std::memcpy(_activeKeys.data(), _animation.getKeyframes().data(), sizeof(CompactKeyframe) * _cursor);
}
