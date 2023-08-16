// author: Changwan Yu
#pragma once
#include <DirectXMath.h>

namespace pa
{
	struct Transform
	{
		DirectX::XMFLOAT4 scale		  = {1.0f, 1.0f, 1.0f, 0.0f};
		DirectX::XMFLOAT4 rotation	  = {0.0f, 0.0f, 0.0f, 1.0f};
		DirectX::XMFLOAT4 translation = {};
		DirectX::XMMATRIX getMatrix() const
		{
			using namespace DirectX;
			const XMVECTOR S = XMLoadFloat4(&scale);
			const XMVECTOR R = XMLoadFloat4(&rotation);
			const XMVECTOR T = XMLoadFloat4(&translation);
			const XMMATRIX M = XMMatrixAffineTransformation(S, XMVectorZero(), R, T);

			return M;
		}
	};
}
