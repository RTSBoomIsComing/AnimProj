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

		void setVertexIndexBuffers(ID3D11DeviceContext* pDeviceContext);

	public:
		UINT getVertexCount() const;
		UINT getIndexCount() const;

	private:
		ComPtr<ID3D11Buffer>	_indexBuffer;
		ComPtr<ID3D11Buffer>	_vertexPositionBuffer;
		ComPtr<ID3D11Buffer>	_vertexColorBuffer;

		UINT _strides[2]	= {};
		UINT _offsets[2]	= {};

		UINT _indexCount	= 0;
		UINT _vertexCount	= 0;
	};

	class MeshFactory
	{
	public:
		static Mesh* CreateCubeMesh(ID3D11Device* pDevice);
		static Mesh* CreateSphereMesh(ID3D11Device* pDevice);
	};
}

