// author: Changwan Yu
#include "pch.h"
#include "Skeleton.h"

DirectX::XMMATRIX pa::Skeleton::getBoneMatrix(size_t index) const
{
    using namespace DirectX;
    return XMLoadFloat4x4(&_boneMatrices[index]);
}
