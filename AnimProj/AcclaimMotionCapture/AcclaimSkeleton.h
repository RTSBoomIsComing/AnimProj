// author: Changwan Yu
#pragma once
#include "Acclaim.h"
namespace pa
{
	class Acclaim::Skeleton
	{
		friend class Acclaim::Motion;

		using BoneID = uint16_t;
		enum class AxisOrder
		{
			XYZ, XZY, YZX, YXZ, ZXY, ZYX
		};
		struct Root
		{
			std::array<Acclaim::DOF, 6> order		= {};
			DirectX::XMFLOAT3			position	= {};
			DirectX::XMFLOAT3			orientation = {};
		};

		struct Bone
		{
			DirectX::XMFLOAT3	direction	= {};
			DirectX::XMFLOAT3	axis		= {};
			std::string			name;
			float				length		= 0.0f;
		};
	public:
		Skeleton(std::wstring const& filePath);
		~Skeleton() = default;

	private:
		Acclaim::Unit			_unit = {};
		Root					_root = {};
		std::vector<Bone>		_boneData;
		std::vector<BoneID>		_hierarchy;
	};
}

