// author: Changwan Yu
#pragma once
namespace pa
{
	class Skeleton;

	struct Bone
	{

		DirectX::XMFLOAT4	axis = {};
		DirectX::XMFLOAT4	direction = {};	// in global coordinate
		float				length = 0.0f;
		char				axisOrder[4] = "XYZ"; // XYZ with \0
	};

	
	class ASF
	{
		friend class AMC;

	public:
		enum class Channel : std::uint8_t
		{
			RX, RY, RZ, TX, TY, TZ, LN, None,
		};

	public:
		ASF(pa::Skeleton* pSkeleton);
		~ASF() = default;
		ASF(const wchar_t* filePath, pa::Skeleton* pSkeleton);

		bool			loadFromFile(const wchar_t* filePath, class pa::Skeleton* pSkeleton);
		std::size_t		getBoneCount() const;

	private:
		void	parseUnits(std::ifstream& stream);
		void	parseRoot(std::ifstream& stream);
		void	parseBoneData(std::ifstream& stream);
		void	parseHierarchy(std::ifstream& stream);
		void	parseDOF(std::istringstream& stream, Bone& bone);

		Channel	getChannel(std::string channelName) const;

	private:
		DirectX::XMMATRIX	EulerRotation(const DirectX::XMFLOAT4& axis, const std::string& order);

	private:
		pa::Skeleton*							pSkeleton;
		DirectX::XMFLOAT4						_rootPosition;
		std::vector<std::string>				_boneNameList;
		std::uint8_t							_boneCount;

	public:
		float									_unitMass = 1.0f;
		float									_unitLength = 1.0f;
		float									_unitAngle = 1.0f;
		std::vector<Channel[6]>					_DOF;
		std::vector<char[3]>					_axisOrder;
	};
}

