#include "pch.h"
#include "Mesh.h"

void pa::Mesh::setVertexIndexBuffers(ID3D11DeviceContext* pDeviceContext)
{
	ID3D11Buffer* const vertexBuffers[] = {
		_vertexPositionBuffer.Get(),
		_vertexColorBuffer.Get(),
	};
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

pa::Mesh* pa::MeshFactory::CreateCubeMesh(ID3D11Device* pDevice, float scale)
{
	using namespace DirectX;
	Mesh* cubeMesh = new Mesh{};

	{
		scale = scale * 0.5f;
		XMVECTOR positions[] = {
			-scale, -scale, -scale, 1.0, //0
			-scale,  scale, -scale, 1.0, //1
			 scale,  scale, -scale, 1.0, //2
			 scale, -scale, -scale, 1.0, //3
			-scale, -scale,  scale, 1.0, //4
			-scale,  scale,  scale, 1.0, //5
			 scale,  scale,  scale, 1.0, //6
			 scale, -scale,  scale, 1.0, //7
		};
		cubeMesh->_vertexCount = ARRAYSIZE(positions);

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(positions);
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = positions;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		checkResult(pDevice->CreateBuffer(&bufferDesc, &initData, &cubeMesh->_vertexPositionBuffer));
	}
	{
		XMVECTOR colors[] = {
			 1,  1,  1, 1, //0
			 1,  1,  1, 1, //1
			 0,  1,  1, 1, //2
			 0,  1,  1, 1, //3
			 1,  1,  1, 1, //4
			 1,  1,  1, 1, //5
			 0,  1,  1, 1, //6
			 0,  1,  1, 1, //7
		};

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(colors);
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = colors;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		checkResult(pDevice->CreateBuffer(&bufferDesc, &initData, &cubeMesh->_vertexColorBuffer));
	}

	{
		// Supply the actual index data.
		unsigned int indices[] = {
			//Front
			0, 1, 2,
			0, 2, 3,

			//Back
			7, 6, 5,
			7, 5, 4,

			//Top
			1, 5, 6,
			1, 6, 2,

			//Bottom
			3, 7, 4,
			3, 4, 0,

			//Left
			3, 2, 6,
			3, 6, 7,

			//Right
			4, 5, 1,
			4, 1, 0,
		};
		cubeMesh->_indexCount = ARRAYSIZE(indices);

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(indices);
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = indices;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		checkResult(pDevice->CreateBuffer(&bufferDesc, &initData, &cubeMesh->_indexBuffer));
	}

	cubeMesh->_offsets[0] = 0;
	cubeMesh->_offsets[1] = 0;

	cubeMesh->_strides[0] = sizeof(DirectX::XMVECTOR);
	cubeMesh->_strides[1] = sizeof(DirectX::XMVECTOR);

	

	return cubeMesh;
}

pa::Mesh* pa::MeshFactory::CreateSphereMesh(ID3D11Device* pDevice)
{
	return nullptr;
}