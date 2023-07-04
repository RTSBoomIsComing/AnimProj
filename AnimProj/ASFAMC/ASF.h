// author: Changwan Yu
#pragma once
namespace pa
{
	class Skeleton;

	struct Bone
	{

		DirectX::XMFLOAT4	direction = {};	// in global coordinate
		float				length = 0.0f;
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
		void	parseUnits(std::istream& stream);
		void	parseRoot(std::istream& stream);
		void	parseBoneData(std::istream& stream);
		void	parseHierarchy(std::istream& stream);
		//void	parseDOF(std::istream& stream, Bone& bone);

		Channel	getChannel(std::string channelName) const;
		std::size_t getBoneIndexFromName(const std::string& boneName) const;

	private:
		DirectX::XMMATRIX	EulerRotation(const float axis[3], const std::string& order);

	private:
		pa::Skeleton*							_pSkeleton;
		DirectX::XMFLOAT4						_rootPosition;
		std::vector<std::string>				_boneNameList;
		std::uint8_t							_boneCount;

	public:
		float									_unitMass = 1.0f;
		float									_unitLength = 1.0f;
		float									_unitAngle = 1.0f;
		std::vector<Channel[6]>					_DOF;
		std::vector<const char*>				_axisOrder;
	};
}

