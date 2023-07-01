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

		int					id = 0;
		std::string			name;

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
		const std::vector<DirectX::XMFLOAT4X4>	getGlobalBoneTransforms() const;
		std::vector<std::uint32_t>				getBoneConnections();

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

		//std::vector<Bone>	_boneData;	// Root is also treated as bone.

		char				_rootOrders[6][3];
		DirectX::XMFLOAT4	_rootPosition;

		std::unordered_map<std::string, Bone>	_boneData;
		std::unordered_map<std::string, std::string> _boneParentMap;

		std::vector<std::string>				_dfsRoute;
		std::unordered_map<std::string, int>	_dfsNameMap;
		std::vector<DirectX::XMFLOAT4X4>		_dfsBoneTransforms;
	};
}

