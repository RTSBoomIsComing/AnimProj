// author: Changwan Yu
#pragma once
#include "Skeleton.h"
#include "CompactAnimation.h"
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
			WALK_UPBODY, WALK_LOBODY, PUNCH_UPBODY, PUNCH_LOBODY, DANCE_UPBODY, DANCE_LOBODY
		};

	public:
		inline Skeleton	const& getDefaultSkeleton() const { return _skeletonList[0]; }
		inline std::vector<Animation> const& getAnimationList() const { return _animationList; }
		inline Animation	const& getAnimation(AnimationIndex index) const { return _animationList[static_cast<size_t>(index)]; }


	private:
		std::vector<Skeleton>				_skeletonList;
		std::vector<Animation>		_animationList;
	};
}


