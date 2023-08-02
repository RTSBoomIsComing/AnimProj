// author: Changwan Yu
#include "pch.h"
#include "Skeleton.h"

void pa::Skeleton::generateBoneMasks()
{
    _upperBodyMask.resize(this->getBoneCount());
    _lowerBodyMask.resize(this->getBoneCount());

    _upperBodyMask[11 /*lower back dummy bone*/] = true;
    
    _lowerBodyMask[31 /*lhipjoint dummy bone*/] = true;
    _lowerBodyMask[32 /*rhipjoint dummy bone*/] = true;

    for (uint16_t boneID : _hierarchy)
    {
        if (0 == boneID)
            continue;

        if (_upperBodyMask[this->getParentBoneID(boneID)])
            _upperBodyMask[boneID] = true;

        if (_lowerBodyMask[this->getParentBoneID(boneID)])
            _lowerBodyMask[boneID] = true;
    }

    _lowerBodyMask[0 /*root*/] = true;
    _lowerBodyMask[33 /*lower back dummy bone*/] = true;

}

void pa::Skeleton::generateBoneToBoneLTs()
{
	using namespace DirectX;
	_boneToBoneLTs.resize(this->getBoneCount());

	for (const size_t boneID : this->getHierarchy())
	{
		const size_t	parentID = this->getParentBoneID(boneID);
		const XMMATRIX	boneMatrix = this->getBoneMatrix(boneID);

		XMVECTOR boneTranslation = boneMatrix.r[3];

		const float boneStickScale = XMVectorGetX(XMVector3Length(boneTranslation));
		if (boneStickScale <= 0.00001f)
		{
			_boneToBoneLTs[boneID] = XMFLOAT4X4{};
			continue;
		}

		const XMVECTOR V0 = XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f };
		const XMVECTOR V1 = XMVector3Normalize(boneTranslation);

		const float		dotProduct = XMVectorGetX(XMVector3Dot(V0, V1));
		const float		angle = std::acosf(dotProduct);
		const XMVECTOR	rotationAxis = XMVector3Cross(V0, V1);

		XMStoreFloat4x4(&_boneToBoneLTs[boneID],
			DirectX::XMMatrixScaling(0.15f, boneStickScale, 0.15f) * XMMatrixRotationAxis(rotationAxis, angle)
			* DirectX::XMMatrixTranslation(0.f, 0.f, 0.f));
	}
}

DirectX::XMMATRIX pa::Skeleton::getBoneMatrix(size_t index) const
{
    return DirectX::XMLoadFloat4x4(&_boneMatrices[index]);
}

DirectX::XMMATRIX pa::Skeleton::getBoneToBoneLT(size_t index) const
{
	return DirectX::XMLoadFloat4x4(&_boneToBoneLTs[index]);
}
