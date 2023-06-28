// author: Changwan Yu
#pragma once

namespace pa
{
	class Mesh
	{
	public:
		void setVertexIndexBuffers(ID3D11DeviceContext* pDeviceContext);

		static Mesh* GetCubeMesh(ID3D11Device* pDevice);
		static Mesh* GetSphereMesh();

	public:
		UINT getVertexCount() const;
		UINT getIndexCount() const;

	private:
		ComPtr<ID3D11Buffer>	_indexBuffer;
		ComPtr<ID3D11Buffer>	_vertexPositionBuffer;
		ComPtr<ID3D11Buffer>	_vertexColorBuffer;

		UINT _strides[2]{};
		UINT _offsets[2]{};

		UINT _indexCount = 0;
		UINT _vertexCount = 0;




	};
}

