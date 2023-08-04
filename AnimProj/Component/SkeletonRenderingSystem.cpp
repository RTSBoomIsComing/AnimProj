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


	//for (int i = 0; i < SkeletonComponent::_s_boneMatrixPool.size(); i = i + 1024)
	//{
	//	const int bufferID = i / 1024;
	//	if (_boneWorldCBuffers.size() <= bufferID)
	//	{
	//		_boneWorldCBuffers.emplace_back();
	//		createDynamicCBuffer(device, &_boneWorldCBuffers.back(), sizeof(DirectX::XMFLOAT4X4) * 1024);
	//	}
	//	
	//	auto& d3dBuffer = _boneWorldCBuffers[bufferID];
	//	const UINT instanceCount = std::min(1024Ui64, SkeletonComponent::_s_boneMatrixPool.size() - i);
	//	uploadDynamicCBuffer(deviceContext, d3dBuffer.Get(), SkeletonComponent::_s_boneMatrixPool.data() + i, instanceCount);
	//}

	//for (int i = 0; i < SkeletonComponent::_s_boneToBoneMatrixPool.size(); i = i + 1024)
	//{
	//	const int bufferID = i / 1024;
	//	if (_boneToBoneWorldCBuffers.size() <= bufferID)
	//	{
	//		_boneToBoneWorldCBuffers.emplace_back();
	//		createDynamicCBuffer(device, &_boneToBoneWorldCBuffers.back(), sizeof(DirectX::XMFLOAT4X4) * 1024);
	//	}

	//	auto& d3dBuffer = _boneToBoneWorldCBuffers[bufferID];
	//	const UINT instanceCount = std::min(1024Ui64, SkeletonComponent::_s_boneToBoneMatrixPool.size() - i);
	//	uploadDynamicCBuffer(deviceContext, d3dBuffer.Get(), SkeletonComponent::_s_boneToBoneMatrixPool.data() + i, instanceCount);
	//}
}

void pa::SkeletonRenderingSystem::render(ID3D11DeviceContext* deviceContext, const Mesh* boneMesh, const Mesh* boneToBoneMesh)
{
	//for (int i = 0; i < SkeletonComponent::_s_boneMatrixPool.size(); i = i + 1024)
	//{
	//	auto& boneWorldCBuffer = _boneWorldCBuffers[i / 1024];
	//	deviceContext->VSSetConstantBuffers(1, 1, boneWorldCBuffer.GetAddressOf());

	//	UINT instanceCount = std::min(1024Ui64, SkeletonComponent::_s_boneMatrixPool.size() - i);
	//	boneMesh->drawInstanced(deviceContext, instanceCount, 0);
	//}
	deviceContext->VSSetShaderResources(0, 1, _SRVBoneWorld.GetAddressOf());
	boneMesh->drawInstanced(deviceContext, SkeletonComponent::_s_boneMatrixPool.size(), 0);

	deviceContext->VSSetShaderResources(0, 1, _SRVBoneToBoneWorld.GetAddressOf());
	boneToBoneMesh->drawInstanced(deviceContext, SkeletonComponent::_s_boneToBoneMatrixPool.size(), 0);


	//for (int i = 0; i < SkeletonComponent::_s_boneToBoneMatrixPool.size(); i = i + 1024)
	//{
	//	auto& boneToBoneWorldCBuffer = _boneToBoneWorldCBuffers[i / 1024];
	//	deviceContext->VSSetConstantBuffers(1, 1, boneToBoneWorldCBuffer.GetAddressOf());

	//	UINT instanceCount = std::min(1024Ui64, SkeletonComponent::_s_boneToBoneMatrixPool.size() - i);
	//	boneToBoneMesh->drawInstanced(deviceContext, instanceCount, 0);
	//}
}
