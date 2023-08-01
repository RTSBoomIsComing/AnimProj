#include "pch.h"
#include "Mesh.h"

pa::Mesh::Mesh(ID3D11Device* device)
{
}

void pa::Mesh::draw(ID3D11DeviceContext* pDeviceContext) const
{
	setGraphicsPipeline(pDeviceContext);
	pDeviceContext->DrawIndexed(getIndexCount(), 0, 0);
}

void pa::Mesh::drawInstanced(ID3D11DeviceContext* pDeviceContext, UINT instanceCount, UINT startInstanceLocation) const
{
	setGraphicsPipeline(pDeviceContext);
	pDeviceContext->DrawIndexedInstanced(getIndexCount(), instanceCount, 0, 0, startInstanceLocation);
}

void pa::Mesh::setGraphicsPipeline(ID3D11DeviceContext* pDeviceContext) const
{
	ID3D11Buffer* const vertexBuffers[] = {
		_vertexPositionBuffer.Get(),
		_vertexColorBuffer.Get(),
	};
	pDeviceContext->IASetPrimitiveTopology(_primitiveTopology);
	pDeviceContext->IASetVertexBuffers(0, ARRAYSIZE(vertexBuffers), vertexBuffers, _strides, _offsets);
	pDeviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

UINT pa::Mesh::getVertexCount() const
{
	return _vertexCount;
}

UINT pa::Mesh::getIndexCount() const
{
	return _indexCount;
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

