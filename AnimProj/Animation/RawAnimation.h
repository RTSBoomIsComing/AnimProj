// author: Changwan Yu
#pragma once
#include "AnimationTrack.h"
namespace pa
{
	struct RawAnimation
	{
		//struct AnimationTrack
		//{
		//	enum class Type : uint16_t
		//	{
		//		Scale,
		//		Rotation,
		//		Translation
		//	};

		//	std::vector<uint16_t>			times;
		//	std::vector<DirectX::XMFLOAT4>	values;
		//	
		//	uint16_t			boneID	: 14;
		//	Type				type	: 2;
		//};

		std::vector<AnimationTrack> _tracks;
	};
}

