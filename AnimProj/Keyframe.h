// author: Changwan Yu
#pragma once

namespace pa
{
	class Keyframe
	{
	public:
		enum class Channel : uint16_t
		{
			Scale, Rotation, Translation, None
		};
	public:
		uint16_t			_keytime	= std::numeric_limits<uint16_t>::max();
		uint16_t			_bone		= std::numeric_limits<uint16_t>::max();
		Channel				_channel	= Channel::None;

		DirectX::XMFLOAT4	_v;
	};
}


