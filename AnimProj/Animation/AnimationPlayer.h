// author: Changwan Yu
#pragma once

namespace pa
{
	class Animation;
	struct CompactKey;
	class AnimationPlayer
	{
	public:
		AnimationPlayer(const Animation& animation);
		~AnimationPlayer() = default;

	public:
		void update(float deltaTime);
		void storePose(std::vector<Transform>& outPose) const;
		void blendPoseWithBase(std::vector<Transform>& basePose, float weight) const;
		void play();
		void stop();
		void reset();

		void setLoop(bool enable);
	public:
		inline float	getRunningRate()	const { return _runningTime * 120 / _duration; }
		inline float	getRunningTime()	const { return _runningTime; }
		inline int		getDuration()		const { return _duration; }
		inline bool		isRunning()			const { return _isRunning; }


	private:
		void initializeActiveKeys();
		void initializeActiveKeysWithMemCopy();

	private:
		const Animation&	_animation;

		std::vector<std::array<CompactKey, 4>>	_activeKeys;

		uint32_t		_cursor = 0;

		const uint16_t	_duration;
		float			_runningTime	= 0.0f;
		float			_runningFrame	= 0.0f;
		bool			_isRunning		= false;
		bool			_isLooping		= true;
	};
}

