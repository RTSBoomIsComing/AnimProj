// author: Changwan Yu
#pragma once

namespace pa
{
	class CompactAnimation;
	struct CompactKeyframe;
	class AnimationPlayer
	{
	public:
		AnimationPlayer(const CompactAnimation& animation);
		~AnimationPlayer() = default;

	public:
		void update(float deltaTime);
		DirectX::XMVECTOR getBoneRotation(uint32_t boneIndex) const;

	private:
		void initializeActiveKeys();

	private:
		const CompactAnimation&	_animation;

		std::vector<std::array<CompactKeyframe, 4>> _activeKeys;
		std::vector<DirectX::XMVECTOR>				_rotations;		//temp

		uint32_t		_cursor = 0;

		const uint16_t	_duration		= 0;
		float			_runningTime	= 0.0f;
		bool			_isRunning		= true;
		bool			_isLooping		= true;
	};
}

