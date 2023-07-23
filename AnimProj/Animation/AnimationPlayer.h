// author: Changwan Yu
#pragma once
namespace pa
{
	class CompactAnimation;
	class AnimationPlayer
	{
	public:
		AnimationPlayer(const CompactAnimation& animation);
		~AnimationPlayer() = default;

	private:
		const CompactAnimation&	_animation;

		float		_runningTime	= 0.0f;
		uint16_t	_duration		= 0;
		bool		_isRunning		= true;
		bool		_isLooping		= true;

	};
}

