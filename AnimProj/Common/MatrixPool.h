// author: Changwan Yu
#pragma once
namespace pa
{
	class MatrixPool
	{
	public:
		MatrixPool(std::vector<DirectX::XMFLOAT4X4>& pool, size_t stride, size_t capacity);
		~MatrixPool();

	public:
		DirectX::XMFLOAT4X4* acquire();
		void release(DirectX::XMFLOAT4X4* ptr);

	private:
		size_t getID(DirectX::XMFLOAT4X4* ptr) const;
		size_t size() const;

	private:
		const size_t						_stride		= 1;
		std::vector<DirectX::XMFLOAT4X4>&	_pool;
		std::vector<size_t>					_ownerIDs;

		void (*reOwnership)(size_t ownerID, DirectX::XMFLOAT4X4* newPtr);	// 이거 맞아??? 이렇게까지 해야해???
	};
}

