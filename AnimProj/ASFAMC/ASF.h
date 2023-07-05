// author: Changwan Yu
#pragma once
namespace pa
{
	class Skeleton;

	struct Bone
	{

		DirectX::XMFLOAT4	direction	= {};	// in global coordinate
		float				length		= 0.0f;
	};
	
	
	class ASF
	{
		friend class AMC;

	public:
		enum class Channel : uint8_t
		{
			RX, RY, RZ, TX, TY, TZ, LN, None,
		};

		struct DOF
		{
			Channel channels[6] = { Channel::None, Channel::None, Channel::None, Channel::None, Channel::None, Channel::None };
		};



	public:
		ASF(pa::Skeleton* pSkeleton);
		~ASF() = default;
		ASF(pa::Skeleton* pSkeleton, const wchar_t* filePath);

		bool			loadFromFile(const wchar_t* filePath, class pa::Skeleton* pSkeleton);
		size_t			getBoneCount() const;

	private:
		void			parseUnits(std::istream& stream);
		void			parseRoot(std::istream& stream);
		void			parseBoneData(std::istream& stream);
		void			parseHierarchy(std::istream& stream);
		//void	parseDOF(std::istream& stream, Bone& bone);

		Channel	getChannel(std::string channelName) const;
		std::size_t getBoneIndexFromName(const std::string& boneName) const;

	private:
		DirectX::XMMATRIX	eulerRotation(const float axis[3], const std::string& order);

	private:
		pa::Skeleton*							_pSkeleton;
		DirectX::XMFLOAT4						_rootPosition;
		std::vector<std::string>				_boneNameList;
		std::uint8_t							_boneCount;

	public:
		float									_unitMass		= 1.0f;
		float									_unitLength		= 1.0f;
		float									_unitAngle		= 1.0f;
		std::vector<DOF>						_DOFs;
		std::vector<const char*>				_axisOrders;
	};
}

