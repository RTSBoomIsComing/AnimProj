// author: Changwan Yu
#include "pch.h"
#include "MyApplication.h"
#include "Camera.h"
#include "Rendering/Mesh.h"

pa::MyApplication::MyApplication()
{
	using namespace DirectX;

	_pCamera = new Camera{};
	_pCubeMesh = MeshFactory::CreateCubeMesh(_device.Get());

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
}

pa::MyApplication::~MyApplication()
{
	if (nullptr != _pCamera)
		delete _pCamera;

	if (nullptr != _pCubeMesh)
		delete _pCubeMesh;
}

void pa::MyApplication::OnUpdate()
{
	{
		// Test for Clear rendertargetview
		static int testFactor = 0;
		_clearColor[0] = std::sinf(testFactor * 0.01f);
		_clearColor[1] = std::sinf(testFactor * 0.02f);
		_clearColor[2] = std::sinf(testFactor++ * 0.04f);
	}

	{
		static int testFactor = 0;
		using namespace DirectX;
		_pCamera->updateEyePosition(XMVECTOR{
			3.0f * std::cosf(0.01f * testFactor),
			3.0f,
			3.0f * std::sinf(0.01f * testFactor++),
			1.0f, });
	}
	_deviceContext->UpdateSubresource(_cameraConstantBuffer.Get(), 0, nullptr, &_pCamera->getMatrices(), 0, 0);
	_deviceContext->VSSetConstantBuffers(0, 1, _cameraConstantBuffer.GetAddressOf());
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

	_pCubeMesh->setVertexIndexBuffers(_deviceContext.Get());
	_deviceContext->DrawIndexed(_pCubeMesh->getIndexCount(), 0, 0);

	_swapChain->Present(1, 0);
}