// author: Changwan Yu
#pragma once
namespace pa
{
	class Skeleton
	{
		friend class ASF;

	public:
		~Skeleton() = default;

	public:
		void generateBoneMasks();


		DirectX::XMMATRIX					getBoneMatrix(size_t index)		const;

		inline size_t						getBoneCount()					const { return _boneMatrices.size(); }
		inline size_t						getParentBoneID(size_t index)	const { return _parentList[index]; }
		inline std::vector<uint16_t> const&	getHierarchy()					const { return _boneHierarchy; }

	private:
		Skeleton() = default;

	private:
		std::vector<DirectX::XMFLOAT4X4>	_boneMatrices;
		std::vector<std::uint16_t>			_parentList;
		std::vector<std::uint16_t>			_boneHierarchy;

		std::vector<bool>					_upperBodyMask;
		std::vector<bool>					_lowerBodyMask;
	};
}

