// author: Changwan Yu
#include "pch.h"
#include "MatrixPool.h"

pa::MatrixPool::MatrixPool(std::vector<DirectX::XMFLOAT4X4>& pool, size_t stride, size_t capacity)
	: _pool(pool)
	, _stride(stride)
{
	_pool.reserve(capacity * _stride);
}

pa::MatrixPool::~MatrixPool() = default;

DirectX::XMFLOAT4X4* pa::MatrixPool::acquire()
{
	using namespace DirectX;

	const size_t originSize = _pool.size();
	const size_t newSize	= originSize + _stride;
	const bool needReOwnership = newSize > _pool.capacity();

	_pool.resize(originSize + _stride, XMFLOAT4X4{});
	if (needReOwnership)
	{
		// TODO: re-ownership
	}

	return &_pool.data()[originSize];
}

void pa::MatrixPool::release(DirectX::XMFLOAT4X4* ptr)
{
	std::copy_n(_pool.end() - _stride, _stride, ptr);

	// TODO: Change Component ID
	// size_t distance = pData - _pool.data();
}

size_t pa::MatrixPool::getID(DirectX::XMFLOAT4X4* ptr) const 
{
	return (ptr - _pool.data()) / _stride; 
}

size_t pa::MatrixPool::size() const
{
	return _pool.size() / _stride;
}
