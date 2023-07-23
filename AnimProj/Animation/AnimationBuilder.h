// author: Changwan Yu
#pragma once
#include "RawAnimation.h"
namespace pa
{
	class Skeleton;
	class CompactAnimation;
	class AnimationBuilder
	{
		struct KeyframeBuilder
		{
			int		 prevKeyTime;
			uint16_t keyTime;
			uint16_t trackID		: 15;
			uint16_t isQuaternion	: 1;
			DirectX::XMFLOAT4 value;
		};

	public:
		AnimationBuilder(const Skeleton& skeleton, const RawAnimation& rawAnimation);
		~AnimationBuilder() = default;

		static RawAnimation::Track	removeDuplicateFrame(RawAnimation::Track const& track);
		static RawAnimation::Track	fitCurveWithCatmullRom(RawAnimation::Track const& track, float threshold = 0.0001f);
		static CompactAnimation		buildCompactAnimation(std::vector<RawAnimation::Track> const& tracks);

	private:
		const Skeleton&		_skeleton;
		const RawAnimation& _rawAnimation;

		static bool SortingKeyframeBuilderLess(const KeyframeBuilder& a, const KeyframeBuilder& b);
	};
}

