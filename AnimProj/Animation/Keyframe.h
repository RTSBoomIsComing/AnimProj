// author: Changwan Yu
#pragma once

namespace pa
{
	class Keyframe
	{
	public:
		uint16_t			_keytime	= std::numeric_limits<uint16_t>::max();
		
		// Would be substitute for quantization
		uint16_t			_bone		= std::numeric_limits<uint16_t>::max();
		DirectX::XMFLOAT4	_v;
	};
}


