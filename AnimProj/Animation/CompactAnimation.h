// author: Chanwan Yu
#pragma once
#include "CompactKeyframe.h"
#include "AnimationTrack.h"
namespace pa
{
	class CompactAnimation
	{
		friend class AnimationBuilder;

	public:
		CompactAnimation() = default;
		~CompactAnimation() = default;

	private:
		std::vector<CompactKeyframe>		_keyframes;
		std::vector<uint16_t>				_trackIndices;
		std::vector<AnimationTrackHeader>	_trackHeaders;
	};
}

