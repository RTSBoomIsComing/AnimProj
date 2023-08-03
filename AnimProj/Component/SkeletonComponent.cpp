// author: Changwan Yu
#include "pch.h"
#include "SkeletonComponent.h"
#include "../Animation/Skeleton.h"
#include "../Rendering/Mesh.h"

pa::SkeletonComponent::SkeletonComponent(ID3D11Device* device, const Skeleton& skeleton)
	: _skeleton(&skeleton)
{
	const size_t boneCount = skeleton.getBoneCount();
	_boneGTs.resize(boneCount);
	_boneToBoneGTs.resize(boneCount);

	createDynamicCBuffer(device, &_boneToBoneWorldCBuffer, sizeof(DirectX::XMFLOAT4X4) * 1024);
	createDynamicCBuffer(device, &_boneWorldCBuffer, sizeof(DirectX::XMFLOAT4X4) * 1024);
}

pa::SkeletonComponent::~SkeletonComponent() = default;
 
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

		//const XMMATRIX boneToBoneLT = XMLoadFloat4x4(&_boneToBoneLTs[boneID]);
		const XMMATRIX boneToBoneLT = _skeleton->getBoneToBoneLT(boneID);
		const XMMATRIX parentGT = (boneID != 0) ?
			XMLoadFloat4x4(&_boneGTs[parentID]) : XMMatrixIdentity();

		XMStoreFloat4x4(&_boneToBoneGTs[boneID],
			boneToBoneLT * parentGT);
	}

	uploadDynamicCBuffer(deviceContext, _boneToBoneWorldCBuffer.Get(), _boneToBoneGTs.data(), (UINT)_boneToBoneGTs.size());
	uploadDynamicCBuffer(deviceContext, _boneWorldCBuffer.Get(), _boneGTs.data(), (UINT)_boneGTs.size());
}



void pa::SkeletonComponent::render(ID3D11DeviceContext* deviceContext, const Mesh* boneMesh, const Mesh* boneToBoneMesh)
{
	deviceContext->VSSetConstantBuffers(1, 1, _boneWorldCBuffer.GetAddressOf());
	boneMesh->drawInstanced(deviceContext, (UINT)_boneGTs.size(), 0);

	deviceContext->VSSetConstantBuffers(1, 1, _boneToBoneWorldCBuffer.GetAddressOf());
	boneToBoneMesh->drawInstanced(deviceContext, (UINT)_boneToBoneGTs.size(), 0);
}
