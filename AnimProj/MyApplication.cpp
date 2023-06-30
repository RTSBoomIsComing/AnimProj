// author: Changwan Yu
#include "pch.h"
#include "MyApplication.h"
#include "Camera.h"
#include "Rendering/Mesh.h"
#include "ASFAMC/ASF.h"
#include "ASFAMC/AMC.h"

pa::MyApplication::MyApplication()
	: Win32Application()
	, D3D11Application(GetHwnd())
{
	using namespace DirectX;

	_pCamera = new Camera{};
	_pCubeMesh = MeshFactory::CreateCubeMesh(_device.Get(), 0.25f);

	std::wstring asfFilePath = _SOLUTIONDIR;
	asfFilePath += LR"(Assets\ASFAMC\07-walk\07-walk.asf)";

	_pASF = new ASF{ asfFilePath.c_str() };
	_boneConnectionIndices =_pASF->getBoneConnections();

	std::wstring amcFilePath = _SOLUTIONDIR;
	amcFilePath += LR"(Assets\ASFAMC\07-walk\07_05-walk.amc)";
	_pAMC = new AMC{ amcFilePath.c_str() };


	initializeGraphicsPipeline();
}

pa::MyApplication::~MyApplication()
{
	if (nullptr != _pCamera)
		delete _pCamera;

	if (nullptr != _pCubeMesh)
		delete _pCubeMesh;

	if (nullptr != _pASF)
		delete _pASF;
}

void pa::MyApplication::OnUpdate()
{
	using namespace DirectX;

	static auto lastTime = std::chrono::high_resolution_clock::now();
	const auto currentTime = std::chrono::high_resolution_clock::now();
	const auto deltaTime = std::chrono::duration<float>(currentTime - lastTime);
	lastTime = currentTime;

	{
		float cameraDistance = 10.f;
		if (keyState[0])
			_cameraRotationFactor += deltaTime.count();
		if (keyState[2])
			_cameraRotationFactor -= deltaTime.count();
		if (keyState[1])
			_cameraHeight += deltaTime.count();
		if (keyState[3])
			_cameraHeight -= deltaTime.count();



		XMVECTOR newEyePosition = XMVECTOR{
			cameraDistance * std::cosf(-XM_PIDIV2 + _cameraRotationFactor * 3),
			_cameraHeight * 15,
			cameraDistance * std::sinf(-XM_PIDIV2 + _cameraRotationFactor * 3),
			1.0f };

		_pCamera->updateEyePosition(newEyePosition);
	}

	_deviceContext->UpdateSubresource(_cameraConstantBuffer.Get(), 0, nullptr, &_pCamera->getMatrices(), 0, 0);
	_deviceContext->VSSetConstantBuffers(0, 1, _cameraConstantBuffer.GetAddressOf());

	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};

		_deviceContext->Map(_meshConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		//  Update the vertex buffer here.
		auto& transforms = _pASF->getGlobalBoneTransforms();
		memcpy(mappedResource.pData, transforms.data(), sizeof(DirectX::XMMATRIX) * transforms.size());
		//  Reenable GPU access to the vertex buffer data.
		_deviceContext->Unmap(_meshConstantBuffer.Get(), 0);
		_deviceContext->VSSetConstantBuffers(1, 1, _meshConstantBuffer.GetAddressOf());
	}
}

void pa::MyApplication::OnRender()
{
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceContext->IASetInputLayout(_inputLayout.Get());

	_deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);
	_deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);

	_deviceContext->RSSetState(_rasterizerState.Get());
	_deviceContext->RSSetViewports(1, &_viewport);

	_deviceContext->OMSetDepthStencilState(_depthStencilState.Get(), 0);

	_pCubeMesh->setGraphicsPipeline(_deviceContext.Get());
	_deviceContext->DrawIndexedInstanced(_pCubeMesh->getIndexCount(), _pASF->getGlobalBoneTransforms().size(), 0, 0, 0);

	// Render bone connection 
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	_deviceContext->IASetIndexBuffer(_boneConnectionIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	UINT strides = 8;
	UINT offsets = 0;
	_deviceContext->IASetVertexBuffers(0, 1, _boneConnectionVertexBuffer.GetAddressOf(), &strides, &offsets);
	_deviceContext->DrawIndexedInstanced(2, _boneConnectionIndices.size(), 0, 0, 0);

	_swapChain->Present(1, 0);
}

void pa::MyApplication::OnKeyDown(UINT8 key)
{
	// left  : 37
	// up    : 38
	// right : 39
	// down  : 40
	keyState[key - 37] = true;
}

void pa::MyApplication::OnKeyUp(UINT8 key)
{
	// left  : 37
	// up    : 38
	// right : 39
	// down  : 40
	keyState[key - 37] = false;
}

void pa::MyApplication::initializeGraphicsPipeline()
{
	{
		ComPtr<ID3DBlob> vertexShaderBlob;
#if defined(DEBUG) || defined(_DEBUG)
		checkResult(D3DReadFileToBlob(L"../X64/Debug/BasicVertexShader.cso", &vertexShaderBlob));
#else
		checkResult(D3DReadFileToBlob(L"../X64/Release/BasicVertexShader.cso", &vertexShaderBlob));
#endif
		checkResult(_device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &_vertexShader));


		D3D11_INPUT_ELEMENT_DESC inputElementDescs[2] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		checkResult(_device->CreateInputLayout(
			inputElementDescs, ARRAYSIZE(inputElementDescs),
			vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &_inputLayout));
}

	{
		ComPtr<ID3DBlob> pixelShaderBlob;
#if defined(DEBUG) || defined(_DEBUG)
		checkResult(D3DReadFileToBlob(L"../X64/Debug/BasicPixelShader.cso", &pixelShaderBlob));
#else
		checkResult(D3DReadFileToBlob(L"../X64/Release/BasicPixelShader.cso", &pixelShaderBlob));
#endif
		checkResult(_device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &_pixelShader));
	}

	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME D3D11_FILL_SOLID
		rasterizerDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE D3D11_CULL_BACK
		rasterizerDesc.FrontCounterClockwise = FALSE;
		checkResult(_device->CreateRasterizerState(&rasterizerDesc, &_rasterizerState));
	}

	{
		// Create camera constant buffer
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(pa::Camera::Matrices);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		checkResult(_device->CreateBuffer(&bufferDesc, nullptr, &_cameraConstantBuffer));
	}
	{
		// Create mesh constant buffer
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * 100;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		checkResult(_device->CreateBuffer(&bufferDesc, nullptr, &_meshConstantBuffer));
	}

	using namespace DirectX;
	
	{
		XMVECTOR positions[] = {
			0.0, 0.0, 0.0, 1.0, //0
			0.0,  0.0, 0.0, 1.0, //1
		};

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

		checkResult(_device->CreateBuffer(&bufferDesc, &initData, &_boneConnectionVertexBuffer));
	}


	{
		// Supply the actual index data.

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(std::uint32_t) * _boneConnectionIndices.size();
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = _boneConnectionIndices.data();
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		checkResult(_device->CreateBuffer(&bufferDesc, &initData, &_boneConnectionIndexBuffer));
	}

}
