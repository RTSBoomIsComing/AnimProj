// author: Changwan Yu
#pragma once
namespace pa
{
	class MatrixPool
	{
	public:
		MatrixPool(size_t stride, size_t capacity);
		~MatrixPool();

	public:
		DirectX::XMFLOAT4X4* acquire();
		void release(DirectX::XMFLOAT4X4* pData);

	private:
		//void*								_owner;
		size_t								_stride	 = 1;
		std::vector<DirectX::XMFLOAT4X4>	_pool;
	};
}

