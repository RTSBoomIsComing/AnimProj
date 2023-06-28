// author: Changwan Yu
#include "pch.h"
#include "MyApplication.h"
#include "Camera.h"

pa::MyApplication::MyApplication()
{
	using namespace DirectX;

	_pCamera = new Camera{};
	_pCamera->initializeCamera(static_cast<float>(getWidth()), static_cast<float>(getHeight()));

	{
		XMVECTOR positions[8] = {
			-1, -1,  1, 1, //0
			 1, -1,  1, 1, //1
			-1,  1,  1, 1, //2
			 1,  1,  1, 1, //3
			-1, -1, -1, 1, //4
			 1, -1, -1, 1, //5
			-1,  1, -1, 1, //6
			 1,  1, -1, 1, //7
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

		checkResult(_device->CreateBuffer(&bufferDesc, &initData, &_vertexPositionBuffer));
	}
	{
		XMVECTOR colors[8] = {
			 0,  0,  0, 1, //0
			 0,  0,  0, 1, //1
			 0,  1,  0, 1, //2
			 0,  1,  0, 1, //3
			 1,  0,  0, 1, //4
			 1,  0,  0, 1, //5
			 1,  1,  0, 1, //6
			 1,  1,  0, 1, //7
		};

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(colors);
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = colors;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		checkResult(_device->CreateBuffer(&bufferDesc, &initData, &_vertexColorBuffer));
	}

	{
		// Supply the actual index data.
		unsigned int indices[] = {
			//Top
			2, 6, 7,
			2, 3, 7,

			//Bottom
			0, 4, 5,
			0, 1, 5,

			//Left
			0, 2, 6,
			0, 4, 6,

			//Right
			1, 3, 7,
			1, 5, 7,

			//Front
			0, 2, 3,
			0, 1, 3,

			//Back
			4, 6, 7,
			4, 5, 7
		};
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(indices);
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = indices;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		checkResult(_device->CreateBuffer(&bufferDesc, &initData, &_indexBuffer));
	}

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
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME; //D3D11_FILL_WIREFRAME D3D11_FILL_SOLID
		rasterizerDesc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE D3D11_CULL_BACK
		rasterizerDesc.FrontCounterClockwise = FALSE;
		checkResult(_device->CreateRasterizerState(&rasterizerDesc, &_rasterizerState));
	}

	{
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
	delete _pCamera;
}

void pa::MyApplication::OnUpdate()
{
	{
		// Test for Clear rendertargetview
		static int testClearColorFactor = 0;
		_clearColor[0] = sinf(testClearColorFactor * 0.01f);
		_clearColor[1] = sinf(testClearColorFactor * 0.02f);
		_clearColor[2] = sinf(testClearColorFactor++ * 0.04f);
	}

	// TODO: Update eyePosition and focusPosition of Camera
	// _pCamera->setEyePosition(...);
	// _pCamera->setFocusPosition(...);
	_pCamera->updateMatrices();
	_deviceContext->UpdateSubresource(_cameraConstantBuffer.Get(), 0, nullptr, &_pCamera->getMatrices(), 0, 0);
	_deviceContext->VSSetConstantBuffers(0, 1, _cameraConstantBuffer.GetAddressOf());
}

void pa::MyApplication::OnRender()
{
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);


	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceContext->IASetInputLayout(_inputLayout.Get());

	constexpr UINT strides[] = { sizeof(DirectX::XMVECTOR), sizeof(DirectX::XMVECTOR) };
	constexpr UINT offsets[] = { 0, 0 };

	ID3D11Buffer* const vertexBuffers[] = {
		_vertexPositionBuffer.Get(),
		_vertexColorBuffer.Get(),
	};
	_deviceContext->IASetVertexBuffers(0, ARRAYSIZE(vertexBuffers), vertexBuffers, strides, offsets);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	_deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);
	_deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);

	_deviceContext->RSSetState(_rasterizerState.Get());
	_deviceContext->RSSetViewports(1, &_viewport);

	_deviceContext->DrawIndexed(36, 0, 0);

	_swapChain->Present(1, 0);
}