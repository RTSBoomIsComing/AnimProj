#include "pch.h"
#include "SkeletonRenderingSystem.h"
#include "../Rendering/Mesh.h"

pa::SkeletonRenderingSystem::SkeletonRenderingSystem(ID3D11Device* device)
{
	createDynamicSBuffer(device, &_boneWorldSBuffer, sizeof(DirectX::XMFLOAT4X4));
	createBufferSRV(device, _boneWorldSBuffer.Get(), &_SRVBoneWorld);

	createDynamicSBuffer(device, &_boneToBoneWorldSBuffer, sizeof(DirectX::XMFLOAT4X4));
	createBufferSRV(device, _boneToBoneWorldSBuffer.Get(), &_SRVBoneToBoneWorld);
}

void pa::SkeletonRenderingSystem::update(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<DirectX::XMFLOAT4X4> const& boneGTs, std::vector<DirectX::XMFLOAT4X4> const& boneToBoneGTs)
{
	_boneCount = boneGTs.size();
	uploadDynamicBuffer(deviceContext, _boneWorldSBuffer.Get(), boneGTs.data(), _boneCount);

	_boneToBoneCount = boneToBoneGTs.size();
	uploadDynamicBuffer(deviceContext, _boneToBoneWorldSBuffer.Get(), boneToBoneGTs.data(), _boneToBoneCount);
}

void pa::SkeletonRenderingSystem::render(ID3D11DeviceContext* deviceContext, const Mesh* boneMesh, const Mesh* boneToBoneMesh)
{
	deviceContext->VSSetShaderResources(0, 1, _SRVBoneWorld.GetAddressOf());
	boneMesh->drawInstanced(deviceContext, _boneCount, 0);

	deviceContext->VSSetShaderResources(0, 1, _SRVBoneToBoneWorld.GetAddressOf());
	boneToBoneMesh->drawInstanced(deviceContext, _boneToBoneCount, 0);
}
