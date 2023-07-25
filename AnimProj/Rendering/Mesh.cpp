#include "pch.h"
#include "Mesh.h"

pa::Mesh::Mesh(ID3D11Device* device)
{
	// Create mesh constant buffer
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * 100;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	checkResult(device->CreateBuffer(&bufferDesc, nullptr, &_worldCBuffer));	
}

void pa::Mesh::draw(ID3D11DeviceContext* pDeviceContext)
{
	setGraphicsPipeline(pDeviceContext);
	pDeviceContext->DrawIndexed(getIndexCount(), 0, 0);
}

void pa::Mesh::drawInstanced(ID3D11DeviceContext* pDeviceContext, UINT instanceCount)
{
	setGraphicsPipeline(pDeviceContext);
	pDeviceContext->DrawIndexedInstanced(getIndexCount(), instanceCount, 0, 0, 0);
}

void pa::Mesh::setGraphicsPipeline(ID3D11DeviceContext* pDeviceContext)
{
	ID3D11Buffer* const vertexBuffers[] = {
		_vertexPositionBuffer.Get(),
		_vertexColorBuffer.Get(),
	};
	pDeviceContext->IASetPrimitiveTopology(_primitiveTopology);
	pDeviceContext->IASetVertexBuffers(0, ARRAYSIZE(vertexBuffers), vertexBuffers, _strides, _offsets);
	pDeviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	pDeviceContext->VSSetConstantBuffers(1, 1, _worldCBuffer.GetAddressOf());
}

UINT pa::Mesh::getVertexCount() const
{
	return _vertexCount;
}

UINT pa::Mesh::getIndexCount() const
{
	return _indexCount;
}

void pa::Mesh::updateInstanceData(ID3D11DeviceContext* deviceContext, const DirectX::XMFLOAT4X4* matrices, UINT count)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext->Map(_worldCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, matrices, sizeof(DirectX::XMFLOAT4X4) * count);
	deviceContext->Unmap(_worldCBuffer.Get(), 0);
}

void pa::Mesh::processVertices(ID3D11Device* pDevice, std::vector<DirectX::XMFLOAT4> const& positions, std::vector<DirectX::XMFLOAT4> const& colors)
{
	assert(!positions.empty());
	assert(positions.size() == colors.size() && "sizes of vertex attributes are not equal");
	_vertexCount = static_cast<UINT>(positions.size());

	// process positions
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(positions.front()) * _vertexCount;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = positions.data();
	checkResult(pDevice->CreateBuffer(&bufferDesc, &initData, &_vertexPositionBuffer));

	// process colors
	bufferDesc.ByteWidth = sizeof(colors.front()) * _vertexCount;
	
	initData = {};
	initData.pSysMem = colors.data();
	checkResult(pDevice->CreateBuffer(&bufferDesc, &initData, &_vertexColorBuffer));

	_strides[0] = sizeof(positions.front());
	_strides[1] = sizeof(colors.front());
}

void pa::Mesh::processIndices(ID3D11Device* pDevice, std::vector<UINT> const& indices)
{
	assert(indices.size());
	_indexCount = static_cast<UINT>(indices.size());

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(indices.front()) * _indexCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = indices.data();
	checkResult(pDevice->CreateBuffer(&bufferDesc, &initData, &_indexBuffer));
}

