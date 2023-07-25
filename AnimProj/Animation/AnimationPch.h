// author: Changwan Yu
#pragma once
#include <DirectXMath.h>
#include <vector>

namespace pa
{
	enum class AnimationTrackType : uint16_t
	{
		Scale,
		Rotation,
		Translation
	};

	struct AnimationTrackHeader
	{
		uint16_t			boneID : 14;
		AnimationTrackType	type : 2;
	};

	struct AnimationTrack
	{
		std::vector<uint16_t>			times;
		std::vector<DirectX::XMFLOAT4>	values;

		uint16_t						boneID : 14;
		AnimationTrackType				type : 2;
	};

	struct RawAnimation
	{
		std::vector<AnimationTrack> _tracks;
	};

}
