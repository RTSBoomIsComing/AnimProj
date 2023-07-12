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

		inline size_t						getBoneCount()						const { return _boneMatrices.size(); }
		inline size_t						getParentBoneIndex(size_t index)	const { return _parentList[index]; }
		inline std::vector<uint8_t> const&	getHierarchy()						const { return _boneHierarchy; }

	private:
		Skeleton() = default;

	private:
		std::vector<DirectX::XMFLOAT4X4>	_boneMatrices;
		std::vector<std::uint8_t>			_parentList;
		std::vector<std::uint8_t>			_boneHierarchy;
	};
}

