// author: Chanwan Yu
#pragma once
#include "CompactKeyframe.h"
namespace pa
{
	class CompactAnimation
	{
		friend class AnimationBuilder;

	public:
		struct TrackHeader
		{
			enum class Type : uint16_t
			{
				Scale,
				Rotation,
				Translation
			};

			uint16_t	boneID : 14;
			Type		type : 2;
		};

	public:
		CompactAnimation() = default;
		~CompactAnimation() = default;

	private:
		std::vector<CompactKeyframe>	_keyframes;
		std::vector<uint16_t>			_trackIndices;
		std::vector<TrackHeader>		_trackHeaders;
	};
}

