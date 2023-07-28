// author: Changwan Yu
#pragma once

namespace pa
{
	bool checkResult(HRESULT result);
	void createDynamicCBuffer(ID3D11Device* device, ID3D11Buffer** ppBuffer, UINT byteWidth);
}