// author: Changwan Yu
#pragma once
namespace pa
{
	class Skeleton
	{
		friend class ASF;
		friend class MyApplication;
	public:
		struct Bone
		{
			// quaternion;
			DirectX::XMFLOAT4	rotation	= { 0.0f, 0.0f, 0.0f, 1.0f };

			// direction * length * unit_length;
			DirectX::XMFLOAT4	direction	= {};
		};

	public:
		Skeleton() = default;
		~Skeleton() = default;

		inline const Skeleton::Bone& getBone(std::size_t index) const { return _boneList[index]; }
		inline size_t getBoneCount() const { return _boneList.size(); }


	private:
		// contain bone data
		std::vector<Skeleton::Bone>	_boneList;

		// indices of parent bone
		std::vector<std::uint8_t>	_parentList;

		// sorted indices of bone along depth first search
		std::vector<std::uint8_t>	_DFSPath;
	};
}

