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
		DirectX::XMVECTOR getBoneRotation(uint32_t boneIndex) const;
		void play();
		void stop();
		void reset();
	public:
		inline float	getRunningRate()	const { return _runningTime / _duration; }
		inline float	getRunningTime()	const { return _runningTime; }
		inline int		getDuration()		const { return _duration; }
		inline bool		isRunning()			const { return _isRunning; }


	private:
		void initializeActiveKeys();
		void initializeActiveKeysWithMemCopy();

	private:
		const Animation&	_animation;

		std::vector<std::array<CompactKey, 4>>	_activeKeys;
		std::vector<DirectX::XMVECTOR>			_rotations;

		uint32_t		_cursor = 0;

		const uint16_t	_duration;
		float			_runningTime	= 0.0f;
		bool			_isRunning		= false;
		bool			_isLooping		= true;
	};
}

