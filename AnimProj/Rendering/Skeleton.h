// author: Changwan Yu
#pragma once
namespace pa
{
	class Skeleton
	{
		friend class ASF;
		friend class AMC;

	public:
		~Skeleton() = default;

		DirectX::XMMATRIX					getBoneMatrix(size_t index)			const;

		//inline Skeleton::Bone		const&	getBone(size_t index)				const { return _boneList[index]; }
		inline size_t						getBoneCount()						const { return _boneMatrices.size(); }
		inline size_t						getParentBoneIndex(size_t index)	const { return _parentList[index]; }
		inline std::vector<uint8_t> const&	getDFSPath()						const { return _boneHierarchy; }

	private:
		Skeleton() = default;

	private:
		// contain bone data
		//std::vector<Skeleton::Bone>	_boneList;

		std::vector<DirectX::XMFLOAT4X4>	_boneMatrices;
		std::vector<std::uint8_t>			_parentList;
		std::vector<std::uint8_t>			_boneHierarchy;
	};
}

