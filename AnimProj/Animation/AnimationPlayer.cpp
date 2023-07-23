// author: Changwan Yu
#include "pch.h"
#include "AnimationPlayer.h"
#include "CompactAnimation.h"

pa::AnimationPlayer::AnimationPlayer(const CompactAnimation& animation)
	: _animation(animation)
	, _duration(_animation.getKeyframes().back().keytime)
{
	_activeKeys.resize(_animation.getTrackHeaders().size());
	initializeActiveKeys();
}

void pa::AnimationPlayer::update(float deltaTime)
{
	using namespace DirectX;
	constexpr int fps = 120;

	if (_isRunning)
		_runningTime += deltaTime;
	
	
	float elipsedFrame = _runningTime * fps;
	if (_isLooping && _duration < elipsedFrame)
	{
		elipsedFrame = 0;
		_runningTime = 0;
		initializeActiveKeys();
	}

	while (_cursor < _animation.getKeyframes().size())
	{
		uint16_t trackID = _animation.getTrackIndices()[_cursor];
		if (_activeKeys[trackID][2].keytime < elipsedFrame)
		{
			_activeKeys[trackID][0] = _activeKeys[trackID][1];
			_activeKeys[trackID][1] = _activeKeys[trackID][2];
			_activeKeys[trackID][2] = _activeKeys[trackID][3];
			_activeKeys[trackID][3] = _animation.getKeyframes()[_cursor];
		
			_cursor++;
		}
		else
			break;
	}
}

void pa::AnimationPlayer::initializeActiveKeys()
{
	const size_t trackCount = _animation.getTrackHeaders().size();
	_cursor = trackCount * 4;
	for (size_t i = 0; i < _cursor; i++)
	{
		const uint16_t trackID = _animation.getTrackIndices()[i];
		
		_activeKeys[trackID][0] = _activeKeys[trackID][1];
		_activeKeys[trackID][1] = _activeKeys[trackID][2];
		_activeKeys[trackID][2] = _activeKeys[trackID][3];
		_activeKeys[trackID][3] = _animation.getKeyframes()[i];	
	}
}
