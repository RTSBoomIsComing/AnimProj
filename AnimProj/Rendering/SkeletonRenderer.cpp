// author: Changwan Yu
#include "pch.h"
#include "SkeletonRenderer.h"
#include "../Animation/Skeleton.h"

pa::SkeletonRenderer::SkeletonRenderer(const Skeleton* skeleton)
	: _skeleton(skeleton)
{
	using namespace DirectX;
	_jointToJointLTs.resize(_skeleton->getBoneCount());

	for (const size_t boneID : _skeleton->getHierarchy())
	{
		const size_t	parentID = _skeleton->getParentBoneID(boneID);
		const XMMATRIX	boneMatrix = _skeleton->getBoneMatrix(boneID);

		XMVECTOR boneTranslation = {};
		XMVECTOR dummyVector = {};
		DirectX::XMMatrixDecompose(&dummyVector, &dummyVector, &boneTranslation, boneMatrix);

		const float boneStickScale = XMVectorGetX(XMVector3Length(boneTranslation));
		if (boneStickScale <= 0.00001f)
		{
			_jointToJointLTs[boneID] = XMFLOAT4X4{};
			continue;
		}

		const XMVECTOR V0 = XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f };
		const XMVECTOR V1 = XMVector3Normalize(boneTranslation);

		const float		dotProduct = XMVectorGetX(XMVector3Dot(V0, V1));
		const float		angle = std::acosf(dotProduct);
		const XMVECTOR	rotationAxis = XMVector3Cross(V0, V1);
		
		
		XMStoreFloat4x4(&_jointToJointLTs[boneID],
			DirectX::XMMatrixScaling(0.15f, boneStickScale, 0.15f) * XMMatrixRotationAxis(rotationAxis, angle)
			* DirectX::XMMatrixTranslation(0.f, 0.f, 0.f));
	}
}

void pa::SkeletonRenderer::render(ID3D11DeviceContext* deviceContext, std::vector<DirectX::XMFLOAT4X4> const& jointGTs, std::vector<DirectX::XMFLOAT4X4>& jointToJointGTs)
{
	assert(jointGTs.size() >= _skeleton->getBoneCount());
	assert(jointToJointGTs.size() >= _skeleton->getBoneCount());

	using namespace DirectX;
	for (const size_t boneID : _skeleton->getHierarchy())
	{
		const size_t parentID = _skeleton->getParentBoneID(boneID);

		const XMMATRIX jointToJointLT = XMLoadFloat4x4(&_jointToJointLTs[boneID]);
		const XMMATRIX parentGT = (boneID != 0) ?
			XMLoadFloat4x4(&jointGTs[parentID]) : XMMatrixIdentity();

		XMStoreFloat4x4(&jointToJointGTs[boneID],
			jointToJointLT * parentGT);
	}
}
