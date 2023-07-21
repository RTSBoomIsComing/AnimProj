// author: Changwan Yu
#pragma once
namespace pa
{
	class Skeleton;

	class ASF
	{
		friend class AMC;
		friend class Animation;

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
		~ASF() = default;
		ASF(const wchar_t* filePath);
		ASF(const std::wstring& filePath);

		pa::Skeleton*				createSkeleton();
	public:
		static DirectX::XMMATRIX	eulerRotation(const float axis[3], const std::string& order);
		inline size_t				getParentBoneIndex(size_t index) const { return _parentList[index]; }

	private:
		bool			loadFromFile(const wchar_t* filePath);
		void			parseUnits(std::istream& stream);
		void			parseRoot(std::istream& stream);
		void			parseBoneData(std::istream& stream);
		void			parseHierarchy(std::istream& stream);

		size_t			getBoneCount() const;
		Channel			getChannel(std::string channelName) const;
		std::size_t		getBoneIndexFromName(const std::string& boneName) const;

		void			correctSkeleton();

	private:
		std::vector<uint16_t>					_hierarchy;
		std::vector<uint16_t>					_parentList;
		std::vector<DirectX::XMFLOAT4>			_boneRotations;
		std::vector<DirectX::XMFLOAT4>			_boneTranslations;
		std::vector<std::string>				_boneNameList;

	public:
		float									_unitMass		= 1.0f;
		float									_unitLength		= 1.0f;
		float									_unitAngle		= 1.0f;
		std::vector<DOF>						_DOFs;
		std::vector<std::string>				_axisOrders;
	};
}

