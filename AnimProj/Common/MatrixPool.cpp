// author: Changwan Yu
#include "pch.h"
#include "MatrixPool.h"

pa::MatrixPool::MatrixPool(size_t stride, size_t capacity)
	: _stride(stride)
{
	_pool.reserve(capacity * _stride);
}

pa::MatrixPool::~MatrixPool()
{
}

DirectX::XMFLOAT4X4* pa::MatrixPool::acquire()
{
	using namespace DirectX;

	const size_t originSize = _pool.size();
	_pool.resize(originSize + _stride, XMFLOAT4X4{});

	return &_pool.data()[originSize];
}

void pa::MatrixPool::release(DirectX::XMFLOAT4X4* pData)
{
	const size_t back = _pool.size() - _stride;
	std::copy_n(_pool.end() - _stride, _stride, pData);

	// TODO: Change Component ID
	// size_t distance = pData - _pool.data();
}