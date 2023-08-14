// author: Changwan Yu
#pragma once
#include "../Animation/AnimationPlayer.h"
namespace pa
{
	class Actor;
	class World;
	class Skeleton;
	class Animation;
	class AnimationComponent
	{
	public:
		AnimationComponent() = default;
		AnimationComponent(const Skeleton& skeleton);

	public:
		//void onUpdate(World& world, Actor& owner, float deltaTime);

		void								 setSkeleton(const Skeleton& skeleton);
		inline std::vector<Transform> const& getResultPose() const { return _resultPose; }

	private:
		enum class AnimPlayerIndex
		{
			Walk_up,
			Walk_lo,
			Run_up,
			Run_lo,
			Punch_up,
			Punch_lo,
			Jump_up,
			Jump_lo,
			Dance_up,
			Dance_lo,
			None
		};

		AnimationPlayer& getAnimationPlayer(AnimPlayerIndex index);

	private:
		const Skeleton* _skeleton = nullptr;

		std::vector<AnimationPlayer> _animationPlayers;
		std::vector<Transform>		 _resultPose;
	};
}
