// author: Changwan Yu
#include "pch.h"
#include "DirectX11Helper.h"
#include <comdef.h>

bool pa::checkResult(HRESULT result)
{
	if (FAILED(result))
	{
		_com_error error(result);
		LPCTSTR errorMessage = error.ErrorMessage();
		::OutputDebugString(errorMessage);
		DebugBreak();
		return false;
	}
	return true;
}

void pa::createDynamicCBuffer(ID3D11Device* device, ID3D11Buffer** ppBuffer, UINT byteWidth)
{
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = byteWidth;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	checkResult(device->CreateBuffer(&bufferDesc, nullptr, ppBuffer));
}

void pa::uploadDynamicCBuffer(ID3D11DeviceContext* deviceContext, ID3D11Buffer* pBuffer, const DirectX::XMFLOAT4X4* matrices, UINT count)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	::memcpy(mappedResource.pData, matrices, sizeof(DirectX::XMFLOAT4X4) * count);
	deviceContext->Unmap(pBuffer, 0);
}

void pa::createStructureBuffer(ID3D11Device* device, ID3D11Buffer** ppBuffer, UINT structSize, UINT count)
{
	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = structSize * count;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = structSize;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = device->CreateBuffer(&desc, nullptr, ppBuffer);
	checkResult(hr);
}

void pa::createBufferSRV(ID3D11Device* device, ID3D11Resource* resource, ID3D11ShaderResourceView** ppView, UINT numElement)
{
	HRESULT hr;
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = numElement; // default is 16384;

	hr = device->CreateShaderResourceView(resource, &desc, ppView);
	checkResult(hr);
}