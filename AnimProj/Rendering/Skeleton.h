#pragma once
namespace pa
{
	class Skeleton
	{
	public:
		struct Bone
		{
			DirectX::XMFLOAT4	rotation	= { 0.0f, 0.0f, 0.0f, 1.0f };	// quaternion;
			DirectX::XMFLOAT4	direction	= {};	// direction * length * unit_length;
		};


	};
}

