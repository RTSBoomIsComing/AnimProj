#include "pch.h"
#include "SkeletonPose.h"
#include "Animation.h"
#include "../Rendering/Skeleton.h"
pa::SkeletonPose::SkeletonPose(Skeleton* skeleton, Animation* animation, uint32_t key)
	: _skeleton(skeleton)
{
	_rotationsPerBone.resize(_skeleton->getBoneCount());
	for (size_t boneIndex = 0; boneIndex < _skeleton->getBoneCount(); boneIndex++)
	{
		DirectX::XMStoreFloat4(&_rotationsPerBone[boneIndex],animation->getBoneRotation(boneIndex));
	}
}

void pa::SkeletonPose::createMatrices()
{
	using namespace DirectX;

	assert(_skeleton->getBoneCount() == _rotationsPerBone.size());

	_matricesPerBone.reserve(_rotationsPerBone.size());
	for (const XMFLOAT4& rotation : _rotationsPerBone)
	{
		_matricesPerBone.push_back(XMMatrixRotationQuaternion(XMLoadFloat4(&rotation)));
	}

	assert(_matricesPerBone.size() == _rotationsPerBone.size());
}

void pa::SkeletonPose::destroyMatrices()
{
	_matricesPerBone.clear();
}

std::vector<DirectX::XMMATRIX> const& pa::SkeletonPose::getMatrices() const
{
	return _matricesPerBone;
}

std::vector<DirectX::XMFLOAT4> const& pa::SkeletonPose::getRotations() const
{
	return _rotationsPerBone;
}

