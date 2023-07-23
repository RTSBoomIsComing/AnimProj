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

	public:
		inline const std::vector<CompactKeyframe>&		getKeyframes()		const { return _keyframes; }
		inline const std::vector<uint16_t>&				getTrackIndices()	const { return _trackIndices; }
		inline const std::vector<AnimationTrackHeader>&	getTrackHeaders()	const { return _trackHeaders; }
	
	private:
		std::vector<CompactKeyframe>		_keyframes;
		std::vector<uint16_t>				_trackIndices;
		std::vector<AnimationTrackHeader>	_trackHeaders;
	};
}

