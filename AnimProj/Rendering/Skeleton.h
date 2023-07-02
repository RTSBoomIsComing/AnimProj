#pragma once
namespace pa
{
	class Skeleton
	{
	public:
		struct Bone
		{
			std::string			name;

			DirectX::XMFLOAT4	rotation = {};
			DirectX::XMFLOAT4	translation = {};
		};


	};
}

