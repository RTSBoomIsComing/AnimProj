// author: Changwan Yu
#pragma once

namespace pa
{
	bool checkResult(HRESULT result);
	void createDynamicCBuffer(ID3D11Device* device, ID3D11Buffer** ppBuffer, UINT byteWidth);
	void uploadDynamicCBuffer(ID3D11DeviceContext* deviceContext, ID3D11Buffer* pBuffer, const DirectX::XMFLOAT4X4* matrices, UINT count);
	void createStructureBuffer(ID3D11Device* device, ID3D11Buffer** ppBuffer, UINT structSize, UINT count = 16384);
	void createBufferSRV(ID3D11Device* device, ID3D11Resource* resource, ID3D11ShaderResourceView** ppView, UINT numElement = 16384);
}