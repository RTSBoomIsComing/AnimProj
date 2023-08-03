// author: Changwan Yu
#pragma once

namespace pa
{
	class SceneComponent
	{
	public:
		SceneComponent();
		~SceneComponent();
	public:
		DirectX::XMFLOAT3 position	= {};
		DirectX::XMFLOAT3 euler		= {};
	};
}

