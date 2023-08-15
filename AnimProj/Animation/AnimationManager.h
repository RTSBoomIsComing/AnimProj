// author: Changwan Yu
#pragma once
#include "Skeleton.h"
#include "Animation.h"
namespace pa
{
	class AnimationManager
	{
	public:
		AnimationManager();
		~AnimationManager() = default;

		static AnimationManager& get()
		{
			static AnimationManager animationManager;
			return animationManager;
		}

		void initialize();

	public:
		enum class AnimationIndex
		{
			Walk_up,
			Walk_lo,
			Run_up,
			Run_lo,
			Idle_up,
			Idle_lo,
			Die_up,
			Die_lo,
			ShootingGun_up,
			ShootingGun_lo
		};

	public:
		inline Skeleton const&				 getDefaultSkeleton() const { return _skeletonList[0]; }
		inline std::vector<Animation> const& getAnimationList() const { return _animationList; }
		inline Animation const&				 getAnimation(AnimationIndex index) const { return _animationList[static_cast<size_t>(index)]; }


	private:
		std::vector<Skeleton>  _skeletonList;
		std::vector<Animation> _animationList;
	};
}
