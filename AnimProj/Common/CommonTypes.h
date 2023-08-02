// author: Changwan Yu
#pragma once
#include <DirectXMath.h>

namespace pa
{
	struct Transform
	{
		DirectX::XMFLOAT4 scale = { 1.0f, 1.0f, 1.0f, 0.0f };
		DirectX::XMFLOAT4 rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMFLOAT4 translation = {};
	};
}
