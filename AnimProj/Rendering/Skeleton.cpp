// author: Changwan Yu
#include "pch.h"
#include "Skeleton.h"

DirectX::XMMATRIX pa::Skeleton::getBoneMatrix(size_t index) const
{
    using namespace DirectX;
    //XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat4(&_boneList[index].direction));
    //XMMATRIX rotation = XMMatrixRotationQuaternion(XMLoadFloat4(&_boneList[index].rotation));
    return XMLoadFloat4x4(&_boneMatrices[index]);
}
