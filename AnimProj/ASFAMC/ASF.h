// author: Changwan Yu
#pragma once
namespace pa
{
	struct Bone
	{
		enum class AxisOrder : std::uint8_t
		{
			XYZ, XZY, YZX, YXZ, ZXY, ZYX,
		};

		enum class DOF : std::uint8_t
		{
			RX, RY, RZ, LN /* length */,
			TX, TY, TZ, None
		};

		DirectX::XMFLOAT4	axis = {};
		DirectX::XMFLOAT4	direction = {};
		std::string			name;
		float				length = 0.0f;
		AxisOrder			axisOrder = AxisOrder::XYZ;
		DOF					dof[4] = { DOF::None, DOF::None, DOF::None, DOF::None };
	};

	class ASF
	{
	public:
		enum class UnitAngle : std::uint8_t
		{
			Degree, Radian
		};

	public:
		ASF() = default;
		~ASF() = default;
		ASF(const wchar_t* filePath);

		bool	loadFromFile(const wchar_t* filePath);

	private:
		void	parseUnits(std::ifstream& stream);
		void	parseRoot(std::ifstream& stream);
		void	parseBoneData(std::ifstream& stream);
		void	parseHierarchy(std::ifstream& stream);
		void	parseAMCRootDataOrder(std::ifstream& stream);
		void	parseRootPosition(std::ifstream& stream);
		void	parseEachBone(std::ifstream& stream, int boneIndex);
		void	parseAxisOrder(std::ifstream& stream, Bone& bone);
		void	parseAxis(std::ifstream& stream, Bone& bone);
		void	parseDOF(std::ifstream& stream, Bone& bone);

	private:
		float				_unitMass;
		float				_unitLength;
		UnitAngle			_unitAngle;
		std::vector<Bone>	_boneData;	// Root is also treated as bone.

		Bone::DOF			_rootOrder[6];
		DirectX::XMFLOAT4	_rootPosition;

		std::unordered_map<std::string, int>	_boneNameMap;
		std::vector<std::string>				_boneParentArray;
	};
}

