// author: Changwan Yu
#include "pch.h"
#include "SkeletonComponent.h"
#include "../Animation/Skeleton.h"
#include "../Rendering/Mesh.h"

std::vector<uint32_t>				pa::SkeletonComponent::_s_ownerIDs;
std::vector<DirectX::XMFLOAT4X4>	pa::SkeletonComponent::_s_boneMatrixPool;
std::vector<DirectX::XMFLOAT4X4>	pa::SkeletonComponent::_s_boneToBoneMatrixPool;
std::vector<std::unique_ptr<pa::SkeletonComponent>>	pa::SkeletonComponent::_s_components;

pa::SkeletonComponent::SkeletonComponent(const Skeleton& skeleton, DirectX::XMFLOAT4X4* boneGTs, DirectX::XMFLOAT4X4* boneToBoneGTs)
	: _skeleton(&skeleton)
	, _boneGTs(boneGTs)
	, _boneToBoneGTs(boneToBoneGTs)
{}

pa::SkeletonComponent::~SkeletonComponent() = default;
 
void pa::SkeletonComponent::create(SkeletonComponent** outComp, const Skeleton& skeleton)
{
	const size_t boneCount = skeleton.getBoneCount();
	const size_t componentID = _s_components.size();
	_s_ownerIDs.push_back(static_cast<uint32_t>(componentID));

	DirectX::XMFLOAT4X4* boneGTs		= _s_boneMatrixPool.data() + _s_boneMatrixPool.size();
	DirectX::XMFLOAT4X4* boneToBoneGTs	= _s_boneToBoneMatrixPool.data() + _s_boneToBoneMatrixPool.size();

	auto skeletonComp = std::make_unique<SkeletonComponent>(skeleton, boneGTs, boneToBoneGTs);
	_s_components.push_back(std::move(skeletonComp));
	*outComp = _s_components.back().get();

	bool needReOwnership = _s_boneMatrixPool.capacity() < _s_boneMatrixPool.size() + boneCount;
	_s_boneMatrixPool.resize(_s_boneMatrixPool.size() + boneCount);
	if (needReOwnership)
	{
		size_t sumBoneCount = 0;
		for (uint32_t componentID : _s_ownerIDs)
		{
			SkeletonComponent& owner = *_s_components[componentID];
			owner._boneGTs = _s_boneMatrixPool.data() + sumBoneCount;
			sumBoneCount += owner._skeleton->getBoneCount();
		}
	}

	needReOwnership = _s_boneToBoneMatrixPool.capacity() < _s_boneToBoneMatrixPool.size() + boneCount;
	_s_boneToBoneMatrixPool.resize(_s_boneToBoneMatrixPool.size() + boneCount);
	if (needReOwnership)
	{
		size_t sumBoneCount = 0;
		for (uint32_t componentID : _s_ownerIDs)
		{
			SkeletonComponent& owner = *_s_components[componentID];
			owner._boneToBoneGTs = _s_boneToBoneMatrixPool.data() + sumBoneCount;
			sumBoneCount += owner._skeleton->getBoneCount();
		}
	}
}

void pa::SkeletonComponent::destroy(SkeletonComponent** skeletonComp)
{
	if (nullptr == *skeletonComp)
		return;

	const size_t boneCount = (*skeletonComp)->_skeleton->getBoneCount();
	
	const uint32_t componentID = _s_ownerIDs.back();
	SkeletonComponent& lastOwner = *_s_components[componentID];

	std::copy_n(lastOwner._boneGTs, boneCount, (*skeletonComp)->_boneGTs);
	std::copy_n(lastOwner._boneToBoneGTs, boneCount, (*skeletonComp)->_boneToBoneGTs);

	lastOwner._boneGTs = (*skeletonComp)->_boneGTs;
	lastOwner._boneToBoneGTs = (*skeletonComp)->_boneToBoneGTs;

	_s_boneMatrixPool.resize(_s_boneMatrixPool.size() - boneCount);
	_s_boneToBoneMatrixPool.resize(_s_boneToBoneMatrixPool.size() - boneCount);

	*skeletonComp = nullptr;
}

void pa::SkeletonComponent::update(const Transform* pose, DirectX::XMVECTOR const& worldPosition, DirectX::XMVECTOR const& QWorldRotation)
{
	using namespace DirectX;
	
	for (const size_t boneID : _skeleton->getHierarchy())
	{
		const XMVECTOR S = XMLoadFloat4(&pose[boneID].scale);
		const XMVECTOR R = XMLoadFloat4(&pose[boneID].rotation);
		const XMVECTOR T = XMLoadFloat4(&pose[boneID].translation);

		const XMMATRIX animationMatrix = XMMatrixAffineTransformation(S, XMVectorZero(), R, T);

		const size_t	parentID = _skeleton->getParentBoneID(boneID);

		XMMATRIX parentWorldTransform = {};
		if (boneID == 0)
		{
			parentWorldTransform = XMMatrixAffineTransformation(
				XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f), XMVectorZero(),
				QWorldRotation, worldPosition);
		}
		else
		{
			parentWorldTransform = XMLoadFloat4x4(&_boneGTs[parentID]);
		}

		XMMATRIX boneMatrix = _skeleton->getBoneMatrix(boneID);
		XMStoreFloat4x4(&_boneGTs[boneID], animationMatrix * boneMatrix * parentWorldTransform);
	}

	for (const size_t boneID : _skeleton->getHierarchy())
	{
		const size_t parentID = _skeleton->getParentBoneID(boneID);

		const XMMATRIX boneToBoneLT = _skeleton->getBoneToBoneLT(boneID);
		const XMMATRIX parentGT = (boneID != 0) ?
			XMLoadFloat4x4(&_boneGTs[parentID]) : XMMatrixIdentity();

		XMStoreFloat4x4(&_boneToBoneGTs[boneID],
			boneToBoneLT * parentGT);
	}
}