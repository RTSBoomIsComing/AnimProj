// author: Changwan Yu
#include "pch.h"
#include "SkeletonComponent.h"
#include "../Animation/Skeleton.h"
#include "../Rendering/Mesh.h"

std::vector<ComPtr<ID3D11Buffer>>	pa::SkeletonComponent::s_boneWorldCBuffer;
std::vector<ComPtr<ID3D11Buffer>>	pa::SkeletonComponent::s_boneToBoneWorldCBuffer;

std::vector<uint32_t>				pa::SkeletonComponent::_s_ownerIDs;
std::vector<DirectX::XMFLOAT4X4>	pa::SkeletonComponent::_s_boneMatrixPool;
std::vector<DirectX::XMFLOAT4X4>	pa::SkeletonComponent::_s_boneToBoneMatrixPool;
std::vector<std::unique_ptr<pa::SkeletonComponent>>	pa::SkeletonComponent::_s_components;

pa::SkeletonComponent::SkeletonComponent(ID3D11Device* device, const Skeleton& skeleton, DirectX::XMFLOAT4X4* boneGTs, DirectX::XMFLOAT4X4* boneToBoneGTs)
	: _skeleton(&skeleton)
	, _boneGTs(boneGTs)
	, _boneToBoneGTs(boneToBoneGTs)
{
	//const size_t boneCount = skeleton.getBoneCount();

	createDynamicCBuffer(device, &_boneToBoneWorldCBuffer, sizeof(DirectX::XMFLOAT4X4) * 1024);
	createDynamicCBuffer(device, &_boneWorldCBuffer, sizeof(DirectX::XMFLOAT4X4) * 1024);
}

pa::SkeletonComponent::~SkeletonComponent() = default;
 
void pa::SkeletonComponent::globalInitialize()
{
	_s_components.reserve(1000);
}

void pa::SkeletonComponent::create(ID3D11Device* device, SkeletonComponent** outComp, const Skeleton& skeleton)
{
	const size_t boneCount = skeleton.getBoneCount();
	const size_t componentID = _s_components.size();
	_s_ownerIDs.push_back(static_cast<uint32_t>(componentID));

	DirectX::XMFLOAT4X4* boneGTs = _s_boneMatrixPool.data() + _s_boneMatrixPool.size();
	DirectX::XMFLOAT4X4* boneToBoneGTs = _s_boneToBoneMatrixPool.data() + _s_boneToBoneMatrixPool.size();

	auto skeletonComp = std::make_unique<SkeletonComponent>(device, skeleton, boneGTs, boneToBoneGTs);
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

	*skeletonComp = nullptr;
}

void pa::SkeletonComponent::update(ID3D11DeviceContext* deviceContext, const Transform* pose, DirectX::XMVECTOR const& worldPosition, DirectX::XMVECTOR const& QWorldRotation)
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

	uploadDynamicCBuffer(deviceContext, _boneToBoneWorldCBuffer.Get(), _boneToBoneGTs, (UINT)_skeleton->getBoneCount());
	uploadDynamicCBuffer(deviceContext, _boneWorldCBuffer.Get(), _boneGTs, (UINT)_skeleton->getBoneCount());
}



void pa::SkeletonComponent::render(ID3D11DeviceContext* deviceContext, const Mesh* boneMesh, const Mesh* boneToBoneMesh)
{
	deviceContext->VSSetConstantBuffers(1, 1, _boneWorldCBuffer.GetAddressOf());
	boneMesh->drawInstanced(deviceContext, (UINT)_skeleton->getBoneCount(), 0);

	deviceContext->VSSetConstantBuffers(1, 1, _boneToBoneWorldCBuffer.GetAddressOf());
	boneToBoneMesh->drawInstanced(deviceContext, (UINT)_skeleton->getBoneCount(), 0);
}
