// author: Changwan Yu
#pragma once
#include "CompactAnimation.h"
namespace pa
{
	class CompactAnimation;
	class AnimationPlayer
	{
	public:
		AnimationPlayer(const CompactAnimation& animation);
		~AnimationPlayer() = default;

	public:
		void update(float deltaTime);

	private:
		void initializeActiveKeys();

	private:
		const CompactAnimation&	_animation;

		std::vector<std::array<CompactKeyframe, 4>> _activeKeys;
		uint32_t		_cursor = 0;

		const uint16_t	_duration		= 0;
		float			_runningTime	= 0.0f;
		bool			_isRunning		= true;
		bool			_isLooping		= true;
	};
}

