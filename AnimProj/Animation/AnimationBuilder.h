// author: Changwan Yu
#pragma once
#include "RawAnimation.h"
namespace pa
{
	class Skeleton;
	class Animation;
	class AnimationBuilder
	{
	public:
		AnimationBuilder(const Skeleton& skeleton, const RawAnimation& rawAnimation);
		~AnimationBuilder() = default;

		static RawAnimation::Track removeDuplicateFrame(RawAnimation::Track const& track);
		static RawAnimation::Track fitCurveWithCatmullRom(RawAnimation::Track const& track, float threshold = 0.0001f);

		void createDetailedAnimation(Animation* animation);
		void createCompactAnimation(Animation* animation);

	private:
		const Skeleton&		_skeleton;
		const RawAnimation& _rawAnimation;
	};
}

