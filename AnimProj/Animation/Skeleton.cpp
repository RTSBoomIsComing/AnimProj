// author: Changwan Yu
#include "pch.h"
#include "Skeleton.h"

void pa::Skeleton::generateBoneMasks()
{
    _upperBodyMask.resize(this->getBoneCount());
    _lowerBodyMask.resize(this->getBoneCount());

    _upperBodyMask[33 /*lower back dummy bone*/] = true;
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

}

DirectX::XMMATRIX pa::Skeleton::getBoneMatrix(size_t index) const
{
    using namespace DirectX;
    return XMLoadFloat4x4(&_boneMatrices[index]);
}
