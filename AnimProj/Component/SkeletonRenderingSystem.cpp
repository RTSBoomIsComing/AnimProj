#include "pch.h"
#include "SkeletonRenderingSystem.h"
#include "SkeletonComponent.h"
#include "../Rendering/Mesh.h"

pa::SkeletonRenderingSystem::SkeletonRenderingSystem(ID3D11Device* device)
{
	createStructureBuffer(device, &_boneWorldSBuffer, sizeof(DirectX::XMFLOAT4X4));
	createBufferSRV(device, _boneWorldSBuffer.Get(), &_SRVBoneWorld);

	createStructureBuffer(device, &_boneToBoneWorldSBuffer, sizeof(DirectX::XMFLOAT4X4));
	createBufferSRV(device, _boneToBoneWorldSBuffer.Get(), &_SRVBoneToBoneWorld);
}
pa::SkeletonRenderingSystem::~SkeletonRenderingSystem() = default;

void pa::SkeletonRenderingSystem::update(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	uploadDynamicCBuffer(deviceContext, _boneWorldSBuffer.Get(), SkeletonComponent::_s_boneMatrixPool.data(), SkeletonComponent::_s_boneMatrixPool.size());
	uploadDynamicCBuffer(deviceContext, _boneToBoneWorldSBuffer.Get(), SkeletonComponent::_s_boneToBoneMatrixPool.data(), SkeletonComponent::_s_boneToBoneMatrixPool.size());
}

void pa::SkeletonRenderingSystem::render(ID3D11DeviceContext* deviceContext, const Mesh* boneMesh, const Mesh* boneToBoneMesh)
{
	deviceContext->VSSetShaderResources(0, 1, _SRVBoneWorld.GetAddressOf());
	boneMesh->drawInstanced(deviceContext, SkeletonComponent::_s_boneMatrixPool.size(), 0);

	deviceContext->VSSetShaderResources(0, 1, _SRVBoneToBoneWorld.GetAddressOf());
	boneToBoneMesh->drawInstanced(deviceContext, SkeletonComponent::_s_boneToBoneMatrixPool.size(), 0);
}
