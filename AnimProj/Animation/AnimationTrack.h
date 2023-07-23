// author: Changwan Yu
#pragma once

namespace pa
{
	enum class AnimationType : uint16_t
	{
		Scale,
		Rotation,
		Translation
	};

	struct AnimationTrackHeader
	{
		uint16_t			boneID : 14;
		AnimationType		type : 2;
	};

	struct AnimationTrack
	{
		enum class Type : uint16_t
		{
			Scale,
			Rotation,
			Translation
		};

		std::vector<uint16_t>			times;
		std::vector<DirectX::XMFLOAT4>	values;

		uint16_t			boneID	: 14;
		Type				type	: 2;
	};
}

