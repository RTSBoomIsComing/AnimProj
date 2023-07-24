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
		inline const std::vector<uint16_t>&				getTrackIDs()	const { return _trackIDs; }
		inline const std::vector<AnimationTrackHeader>&	getTrackHeaders()	const { return _trackHeaders; }
	
	private:
		std::vector<CompactKeyframe>		_keyframes;
		std::vector<uint16_t>				_trackIDs;
		std::vector<AnimationTrackHeader>	_trackHeaders;
	};
}

