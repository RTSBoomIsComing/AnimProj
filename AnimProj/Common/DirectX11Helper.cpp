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

