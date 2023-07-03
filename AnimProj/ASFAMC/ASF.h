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
		DirectX::XMFLOAT4	direction = {};	// in global coordinate
		float				length = 0.0f;
		char				axisOrder[4] = {}; // XYZ with \0
		DOF					dof[4] = { DOF::None, DOF::None, DOF::None, DOF::None };
	};

	class ASF
	{
		friend class AMC;
	public:
		ASF() = default;
		~ASF() = default;
		ASF(const wchar_t* filePath);

		bool			loadFromFile(const wchar_t* filePath);
		std::size_t		getBoneCount() const;

	private:
		void	parseUnits(std::ifstream& stream);
		void	parseRoot(std::ifstream& stream);
		void	parseBoneData(std::ifstream& stream);
		void	parseHierarchy(std::ifstream& stream);
		void	parseDOF(std::istringstream& stream, Bone& bone);

	private:
		DirectX::XMMATRIX	EulerRotation(const DirectX::XMFLOAT4& axis, const std::string& order);

	private:

		char				_rootOrders[6][3];
		DirectX::XMFLOAT4	_rootPosition;
		std::vector<std::string>				_boneNameList;
		std::vector<DirectX::XMMATRIX>			_globalTransforms;
		std::vector<DirectX::XMMATRIX>			_boneLocalTranslations;
		std::vector<DirectX::XMMATRIX>			_boneLocalRotations;

	public:
		struct Unit
		{
			float	mass = 1.0f;
			float	length = 1.0f;
			float	angle = 1.0f;
		} _unit;

		// Temporarily open these members to public.
		// TODO: Close these members to private and provide public functions.
		std::vector<Bone>						_boneData;
		std::vector<int>						_boneParentList;
		std::vector<int>						_dfsRoute;
		std::vector<DirectX::XMMATRIX>			_globalRotations;
	};
}

