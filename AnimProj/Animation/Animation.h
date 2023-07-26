// author: Chanwan Yu
#pragma once
#include "CompactKey.h"

namespace pa
{
	class Animation
	{
		friend class AnimationBuilder;

	public:
		Animation() = default;
		~Animation() = default;

	public:
		inline const std::vector<CompactKey>&			getKeyframes()		const { return _keyframes; }
		inline const std::vector<uint16_t>&				getTrackIDs()		const { return _trackIDs; }
		inline const std::vector<AnimationTrackHeader>&	getTrackHeaders()	const { return _trackHeaders; }
	
	private:
		std::vector<CompactKey>				_keyframes;
		std::vector<uint16_t>				_trackIDs;
		std::vector<AnimationTrackHeader>	_trackHeaders;
	};
}

