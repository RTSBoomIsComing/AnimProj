// author: Changwan Yu
#pragma once
#include "Rendering.h"
namespace pa
{
	class Mesh
	{
	public:
		Mesh() = default;
		~Mesh() = default;

		void draw(ID3D11DeviceContext* pDeviceContext);
		void drawInstanced(ID3D11DeviceContext* pDeviceContext, UINT instanceCount);
		void setGraphicsPipeline(ID3D11DeviceContext* pDeviceContext);

	public:
		UINT getVertexCount() const;
		UINT getIndexCount() const;
	protected:
		void processVertices(ID3D11Device* pDevice, std::vector<DirectX::XMFLOAT4> const& positions, std::vector<DirectX::XMFLOAT4> const& colors);
		void processIndices(ID3D11Device* pDevice, std::vector<UINT> const& indices);
	protected:
		D3D11_PRIMITIVE_TOPOLOGY	_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		ComPtr<ID3D11Buffer>		_indexBuffer;
		ComPtr<ID3D11Buffer>		_vertexPositionBuffer;
		ComPtr<ID3D11Buffer>		_vertexColorBuffer;

		UINT _strides[2]	= {};
		UINT _offsets[2]	= {};

		UINT _indexCount	= 0;
		UINT _vertexCount	= 0;
	};
}

