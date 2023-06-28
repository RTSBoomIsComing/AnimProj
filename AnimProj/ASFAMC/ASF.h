// author: Changwan Yu
#pragma once
namespace pa
{
	struct Bone
	{
		enum class RotationAxisOrder : std::uint8_t
		{
			XYZ, XZY, YZX, YXZ, ZXY, ZYX,
		};

		DirectX::XMFLOAT4	rotation = {};
		DirectX::XMFLOAT4	direction = {};
		std::string			name;
		float				length = 0.0f;
		RotationAxisOrder	axis = RotationAxisOrder::XYZ;
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

		bool	loadASFFromFile(const wchar_t* filePath);

	private:
		void	ParseUnits(std::ifstream& stream);
		void	ParseRoot(std::ifstream& stream);
		void	ParseBoneData(std::ifstream& stream);
		void	ParseHierarchy(std::ifstream& stream);
		void	ParseAMCRootDataOrder(std::ifstream& stream);
		void	ParseRotationAxisOrder(std::ifstream& stream, Bone& bone);

	private:
		float				_unitMass;
		float				_unitLength;
		UnitAngle			_unitAngle;
		std::vector<Bone>	_boneData;	// Root is also treated as bone.

		std::uint8_t		_amcRootOrder[6];
		DirectX::XMFLOAT4	_rootPosition;
	};
}

