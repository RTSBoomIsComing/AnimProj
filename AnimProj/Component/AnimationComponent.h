// author: Changwan Yu
#pragma once
#include "../Animation/AnimationPlayer.h"
namespace pa
{
	class Skeleton;
	class Animation;
	class Character;
	class AnimationComponent
	{
	public:
		AnimationComponent(ID3D11Device* device, const Skeleton& skeleton);
		~AnimationComponent();

	public:
		void update(Character& owner, float deltaTime);

		inline std::vector<Transform> const& getResultPose() const { return _resultPose; }

	private:
		enum class AnimPlayerIndex
		{
			Walk_up, Walk_lo, Run_up, Run_lo, Punch_up, Punch_lo, Jump_up, Jump_lo, Dance_up, Dance_lo
		};

		AnimationPlayer& getAnimationPlayer(AnimPlayerIndex index);

	private:
		const Skeleton* _skeleton = nullptr;

		std::vector<AnimationPlayer>	_animationPlayers;
		std::vector<Transform>			_resultPose;
	};
}

