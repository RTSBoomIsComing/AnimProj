// author: Changwan Yu
#include "pch.h"
#include "MyApplication.h"
#include "Rendering/Camera.h"
#include "Rendering/Mesh.h"
#include "ASFAMC/ASF.h"
#include "ASFAMC/AMC.h"

pa::MyApplication::MyApplication()
{
	using namespace DirectX;

	initialize(getHwnd());

	_pCamera = new Camera();
	_pCubeMesh = MeshFactory::CreateCubeMesh(_device.Get(), 0.25f);

	std::wstring asfFilePath = _SOLUTIONDIR;
	//asfFilePath += LR"(Assets\ASFAMC\07-walk\07-walk.asf)";
	//asfFilePath += LR"(Assets\ASFAMC\09-run\09-run.asf)";
	//asfFilePath += LR"(Assets\ASFAMC\131-dance\131-dance.asf)";
	asfFilePath += LR"(Assets\ASFAMC\135-martialArts\135-martialArts.asf)";

	std::wstring amcFilePath = _SOLUTIONDIR;
	//amcFilePath += LR"(Assets\ASFAMC\07-walk\07_05-walk.amc)";
	//amcFilePath += LR"(Assets\ASFAMC\09-run\09_06-run.amc)";
	//amcFilePath += LR"(Assets\ASFAMC\131-dance\131_04-dance.amc)";
	amcFilePath += LR"(Assets\ASFAMC\135-martialArts\135_06-martialArts.amc)";

	_pASF = new ASF(asfFilePath.c_str());
	_pAMC = new AMC(amcFilePath.c_str());
	_pAMC->generateAnimation(_pASF);

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

	if (nullptr != _pAMC)
		delete _pAMC;
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

	static std::size_t frameNumber = -1;
	if (frameNumber >= _pAMC->_animationSheets.size() - 1)
		frameNumber = -1;

	frameNumber++;

	std::vector<XMMATRIX> worldTransforms(_pASF->getBoneCount());
	for (const int boneIndex : _pASF->_dfsRoute)
	{
		// Get parent bone data
		const int parentBoneIndex = _pASF->_boneParentList[boneIndex];
		const XMMATRIX& parentWorldTransform = (parentBoneIndex < 0) ? XMMatrixIdentity() : worldTransforms[parentBoneIndex];

		// Get current bone data
		const Bone& bone = _pASF->_boneData[boneIndex];
		const XMVECTOR originalDirection = XMLoadFloat4(&bone.direction) * bone.length * _pASF->_unit.length;
		const XMMATRIX originalRotation = _pASF->_globalRotations[boneIndex];

		// Apply animation
		const XMMATRIX& relativeRotation = _pAMC->_animationSheets[frameNumber].rotations[boneIndex];

		const XMMATRIX localRotation =
			XMMatrixInverse(nullptr, originalRotation) * relativeRotation * originalRotation;

		const XMVECTOR relativePosition = XMVector4Transform(originalDirection, localRotation);
		const XMMATRIX localTranslation = XMMatrixTranslationFromVector(relativePosition);
		const XMMATRIX localTransform = localRotation * localTranslation;

		// Store world transform for rendering
		worldTransforms[boneIndex] = localTransform * parentWorldTransform;
	}

	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};

		_deviceContext->Map(_meshConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		//  Update the vertex buffer here.
		std::memcpy(mappedResource.pData, worldTransforms.data(), sizeof(DirectX::XMMATRIX) * worldTransforms.size());

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

	UINT instanceCount = static_cast<UINT>(_pASF->getBoneCount());
	_deviceContext->DrawIndexedInstanced(_pCubeMesh->getIndexCount(), instanceCount, 0, 0, 0);

	_swapChain->Present(1, 0);
}

void pa::MyApplication::OnKeyDown(UINT8 key)
{
	// left  : 37
	// up    : 38
	// right : 39
	// down  : 40
	if (37 <= key)
		keyState[key - 37] = true;
}

void pa::MyApplication::OnKeyUp(UINT8 key)
{
	// left  : 37
	// up    : 38
	// right : 39
	// down  : 40
	if (37 <= key)
		keyState[key - 37] = false;
}

void pa::MyApplication::initializeGraphicsPipeline()
{
	{
		std::wstring csoFilePath = _OUTDIR;
		csoFilePath.append(L"BasicVertexShader.cso");

		ComPtr<ID3DBlob> vertexShaderBlob;
		checkResult(D3DReadFileToBlob(csoFilePath.c_str(), &vertexShaderBlob));
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
		std::wstring csoFilePath = _OUTDIR;
		csoFilePath.append(L"BasicPixelShader.cso");

		ComPtr<ID3DBlob> pixelShaderBlob;
		checkResult(D3DReadFileToBlob(csoFilePath.c_str(), &pixelShaderBlob));
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
}

void pa::MyApplication::initialize(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = 0;
	swapChainDesc.BufferDesc.Height = 0;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	const D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
	};
	D3D_FEATURE_LEVEL featureLevel = {};
	checkResult(D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
		&swapChainDesc, _swapChain.GetAddressOf(), _device.GetAddressOf(), &featureLevel, _deviceContext.GetAddressOf()));

	ComPtr<ID3D11Texture2D> backBuffer;
	checkResult(_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
	checkResult(_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &_renderTargetView));


	_viewport = CD3D11_VIEWPORT(backBuffer.Get(), _renderTargetView.Get());

	{
		// Create the depth stencil buffer
		D3D11_TEXTURE2D_DESC backBufferDesc;
		backBuffer->GetDesc(&backBufferDesc);
		CD3D11_TEXTURE2D_DESC depthStencilBufferDesc(
			DXGI_FORMAT_D24_UNORM_S8_UINT, backBufferDesc.Width, backBufferDesc.Height,
			1, 0, D3D11_BIND_DEPTH_STENCIL);

		ComPtr<ID3D11Texture2D> depthStencilBuffer;
		checkResult(_device->CreateTexture2D(&depthStencilBufferDesc, nullptr, &depthStencilBuffer));

		// Create the main depth stencil view
		CD3D11_DEPTH_STENCIL_VIEW_DESC dsviewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
		checkResult(_device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsviewDesc, &_depthStencilView));
	}

	{
		// Create depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
		checkResult(_device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilState));
	}
}
