// author: Changwan Yu
#pragma once
#include "Acclaim.h"
namespace pa
{
	class Acclaim::Skeleton
	{
		friend class Acclaim::Motion;

		struct Root
		{
			DirectX::XMFLOAT3			position	= {};
			DirectX::XMFLOAT3			orientation = {};
			std::vector<Acclaim::DOF>	order		= {};
			Acclaim::AxisOrder			axis		= {};
		};

		struct Bone
		{
			std::string					name;
			std::vector<Acclaim::DOF>	dof;
			DirectX::XMFLOAT3			direction	= {};
			DirectX::XMFLOAT3			axis		= {};
			float						length		= 0.0f;
			Acclaim::AxisOrder			axisOrder	= {};
		};
	public:
		Skeleton(std::wstring const& filePath);
		~Skeleton() = default;

	private:
		void			parseUnits(std::istream& stream);
		void			parseRoot(std::istream& stream);
		void			parseBoneData(std::istream& stream);
		void			parseHierarchy(std::istream& stream);

	private:
		Acclaim::Unit				_unit = {};
		Root						_root = {};
		std::vector<Bone>			_boneData;
		std::vector<uint16_t>		_parents;
		std::vector<uint16_t>		_hierarchy;
	};
}

