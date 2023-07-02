// author: Changwan Yu
#pragma once
namespace pa
{
	struct Bone
	{
		enum class DOF : std::uint8_t
		{
			RX, RY, RZ, LN /* length */,
			TX, TY, TZ, None
		};

		DirectX::XMFLOAT4	axis = {};
		DirectX::XMFLOAT4	direction = {};
		float				length = 0.0f;
		char				axisOrder[4] = {}; // XYZ with \0
		DOF					dof[4] = { DOF::None, DOF::None, DOF::None, DOF::None };
	};

	class ASF
	{
	public:
		ASF() = default;
		~ASF() = default;
		ASF(const wchar_t* filePath);

		bool	loadFromFile(const wchar_t* filePath);
		const	std::vector<DirectX::XMMATRIX>	getGlobalBoneTransforms() const;

	private:
		void	parseUnits(std::ifstream& stream);
		void	parseRoot(std::ifstream& stream);
		void	parseBoneData(std::ifstream& stream);
		void	parseHierarchy(std::ifstream& stream);
		void	parseDOF(std::istringstream& stream, Bone& bone);

	private:
		DirectX::XMMATRIX	EulerRotation(const DirectX::XMFLOAT4& axis, const std::string& order);

	private:
		struct Unit
		{
			float	mass = 1.0f;
			float	length = 1.0f;
			float	angle = 1.0f;
		} _unit;

		char				_rootOrders[6][3];
		DirectX::XMFLOAT4	_rootPosition;
		std::vector<std::string>				_boneNameList;
		std::vector<int>						_boneParentList;
		std::vector<Bone>						_boneData2;

		std::vector<int>						_dfsRoute2;

		std::vector<DirectX::XMMATRIX>			_globalTransforms;
		std::vector<DirectX::XMMATRIX>			_globalRotations;
	};
}

