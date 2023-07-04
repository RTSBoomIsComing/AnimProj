// author: Changwan Yu
#pragma once

namespace pa
{
	class MeshFactory;
	class Mesh
	{
		friend MeshFactory;
	public:
		Mesh() = default;
		~Mesh() = default;

		void setGraphicsPipeline(ID3D11DeviceContext* pDeviceContext);

	public:
		UINT getVertexCount() const;
		UINT getIndexCount() const;

	private:
		D3D11_PRIMITIVE_TOPOLOGY	_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		ComPtr<ID3D11Buffer>		_indexBuffer;
		ComPtr<ID3D11Buffer>		_vertexPositionBuffer;
		ComPtr<ID3D11Buffer>		_vertexColorBuffer;

		UINT _strides[2]	= {};
		UINT _offsets[2]	= {};

		UINT _indexCount	= 0;
		UINT _vertexCount	= 0;
	};

	class MeshFactory
	{
	public:
		static Mesh* CreateCubeMesh(ID3D11Device* pDevice, float scale = 1.0f);
		static Mesh* CreateSphereMesh(ID3D11Device* pDevice);
	};
}

