// author: Changwan Yu
#include "pch.h"
#include "Animation.h"


void pa::Animation::initialize(size_t frameCount, size_t boneCount)
{
	_frameCount	= frameCount;
	_boneCount	= boneCount;

	size_t maxSize = _frameCount * _boneCount;

	_rotations.resize(maxSize);
	_positions.resize(maxSize);
}

DirectX::XMFLOAT4 const& pa::Animation::getRotation(std::size_t frameIndex, std::size_t boneIndex) const
{
	return _rotations[frameIndex * getBoneCount() + boneIndex]; 
}

DirectX::XMFLOAT4 const& pa::Animation::getPosition(std::size_t frameIndex, std::size_t boneIndex) const
{
	return _positions[frameIndex * getBoneCount() + boneIndex];
}

DirectX::XMFLOAT4& pa::Animation::getRotation(std::size_t frameIndex, std::size_t boneIndex)
{
	return _rotations[frameIndex * getBoneCount() + boneIndex];
}

DirectX::XMFLOAT4& pa::Animation::getPosition(std::size_t frameIndex, std::size_t boneIndex)
{
	return _positions[frameIndex * getBoneCount() + boneIndex];
}

