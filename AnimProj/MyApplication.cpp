// author: Changwan Yu
#include "pch.h"
#include "MyApplication.h"
#include "Rendering/Camera.h"
#include "Rendering/Mesh.h"
#include "Rendering/StickMesh.h"
#include "Rendering/CubeMesh.h"
#include "Rendering/Skeleton.h"
#include "Rendering/Character.h"
#include "ASFAMC/ASF.h"
#include "ASFAMC/AMC.h"
#include "Animation/Animation.h"


pa::MyApplication::MyApplication()
{
	using namespace DirectX;

	initializeD3dDevices(getHwnd());
	initializeGraphicsPipeline();

	_camera		= new Camera(_device.Get());
	_pStickMesh	= new StickMesh(_device.Get());
	_pCubeMesh	= new CubeMesh(_device.Get(), 0.25f);


	_pSkeleton = new Skeleton();
	std::wstring asfFilePath = _SOLUTIONDIR;
	ASF asf(_pSkeleton, asfFilePath + LR"(Assets\ASFAMC\subject02\02.asf)");

	_character = new Character{};

	std::wstring amcDirectory = _SOLUTIONDIR;
	amcDirectory += LR"(Assets\ASFAMC\subject02\)";

	AMC amcIdle(amcDirectory + LR"(idle.amc)");
	AMC amcWalk(amcDirectory + LR"(walk.amc)");
	AMC amcRun(amcDirectory + LR"(runjog.amc)");
	AMC amcJump(amcDirectory + LR"(jumpbalance.amc)");
	AMC amcPunch(amcDirectory + LR"(punchstrike.amc)");

	_animations.push_back(Animation(&asf, &amcIdle));
	_animations.push_back(Animation(&asf, &amcRun));
	_animations.push_back(Animation(&asf, &amcJump));
	_animations.push_back(Animation(&asf, &amcWalk));
	_animations.push_back(Animation(&asf, &amcPunch));


	//_animations[1].compressAnimation();

	_worldTransforms.resize(_pSkeleton->getBoneCount());
	_boneStickTransforms.resize(_pSkeleton->getBoneCount());
}

pa::MyApplication::~MyApplication()
{
	if (nullptr != _camera)
		delete _camera;

	if (nullptr != _pCubeMesh)
		delete _pCubeMesh;

	if (nullptr != _pStickMesh)
		delete _pStickMesh;

	if (nullptr != _pSkeleton)
		delete _pSkeleton;

	if (nullptr != _character)
		delete _character;
}

void pa::MyApplication::OnUpdate()
{
	using namespace DirectX;

	constexpr int animationIndex = 1;

	
	_timer.update();
	processInput(_timer.getDeltaTime());

	_camera->update(_deviceContext.Get());


	constexpr int	animationFrameRate = 120;
	static float	playTime = 0.0f;

	int keyFrameIndex = static_cast<int>(playTime * animationFrameRate);
	playTime += _timer.getDeltaTime();

	if (keyFrameIndex > _animations[animationIndex]._duration)
	{
		keyFrameIndex = 0;
		playTime = 0.0f;
	}

	for (const size_t boneIndex : _pSkeleton->getDFSPath())
	{
		XMVECTOR finalQuaternion = { 0.0f, 0.0f,0.0f, 1.0f };
		if (false == _animations[animationIndex]._boneAnimation[boneIndex].rotation.empty())
		{
			finalQuaternion =
				_animations[animationIndex].getBoneRotation(boneIndex, keyFrameIndex);

			finalQuaternion = XMQuaternionNormalize(finalQuaternion);
			finalQuaternion = XMQuaternionNormalize(
				XMQuaternionSlerp(XMLoadFloat4(&_animations[0]._boneAnimation[boneIndex].rotation[0].v), finalQuaternion, 1.0f));
		}

		XMMATRIX animationRotation = XMMatrixRotationQuaternion(finalQuaternion);


		const size_t parentBoneIndex = _pSkeleton->getParentBoneIndex(boneIndex);
		const XMMATRIX& parentWorldTransform =
			(parentBoneIndex < _pSkeleton->getBoneCount()) ? _worldTransforms[parentBoneIndex] : XMMatrixIdentity();

		const Skeleton::Bone& bone = _pSkeleton->getBone(boneIndex);
		const XMVECTOR bonePosition = XMLoadFloat4(&bone.direction);
		const XMMATRIX boneRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&bone.rotation));
		XMMATRIX boneMatrix = boneRotation * XMMatrixTranslationFromVector(bonePosition);
		boneMatrix = animationRotation * boneMatrix;

		_worldTransforms[boneIndex] = boneMatrix * parentWorldTransform;

		const float boneStickScale = XMVectorGetX(XMVector3Length(bonePosition));
		if (boneStickScale <= 0)
		{
			_boneStickTransforms[boneIndex] = XMMATRIX{};
			continue;
		}

		const XMVECTOR vec0 = XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f };
		const XMVECTOR vec1 = XMVector3Normalize(bonePosition);

		const float		dotProduct = XMVectorGetX(XMVector3Dot(vec0, vec1));
		const float		angle = std::acosf(dotProduct);
		const XMVECTOR	rotationAxis = XMVector3Cross(vec0, vec1);


		_boneStickTransforms[boneIndex] =
			XMMatrixScaling(0.15f, boneStickScale, 0.15f) * XMMatrixRotationAxis(rotationAxis, angle)
			* XMMatrixTranslation(0.f, 0.f, 0.f) * parentWorldTransform;
	}
}

void pa::MyApplication::OnRender()
{
	// rendering screen 
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	_deviceContext->RSSetViewports(1, &_viewport);
	_deviceContext->OMSetDepthStencilState(_depthStencilState.Get(), 0);

	// renderer
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceContext->IASetInputLayout(_inputLayout.Get());
	_deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);
	_deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);
	_deviceContext->RSSetState(_rasterizerState.Get());

	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};

		_deviceContext->Map(_meshConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		//  Update the vertex buffer here.
		std::memcpy(mappedResource.pData, _boneStickTransforms.data(), sizeof(DirectX::XMMATRIX) * _boneStickTransforms.size());

		//  Reenable GPU access to the vertex buffer data.
		_deviceContext->Unmap(_meshConstantBuffer.Get(), 0);
		_deviceContext->VSSetConstantBuffers(1, 1, _meshConstantBuffer.GetAddressOf());
	}
	_pStickMesh->drawInstanced(_deviceContext.Get(), static_cast<UINT>(_pSkeleton->getBoneCount()));

	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};

		_deviceContext->Map(_meshConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		//  Update the vertex buffer here.
		std::memcpy(mappedResource.pData, _worldTransforms.data(), sizeof(DirectX::XMMATRIX) * _worldTransforms.size());

		//  Reenable GPU access to the vertex buffer data.
		_deviceContext->Unmap(_meshConstantBuffer.Get(), 0);
		_deviceContext->VSSetConstantBuffers(1, 1, _meshConstantBuffer.GetAddressOf());
	}
	_pCubeMesh->drawInstanced(_deviceContext.Get(), static_cast<UINT>(_pSkeleton->getBoneCount()));

	// renderer
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

void pa::MyApplication::processInput(float deltaTime)
{
	using namespace DirectX;
	float cameraDistance = 10.f;
	if (keyState[0])
		_cameraRotationFactor += deltaTime;
	if (keyState[2])
		_cameraRotationFactor -= deltaTime;
	if (keyState[1])
		_cameraHeight += deltaTime;
	if (keyState[3])
		_cameraHeight -= deltaTime;



	XMVECTOR newEyePosition = XMVECTOR{
		cameraDistance * std::cosf(-XM_PIDIV2 + _cameraRotationFactor * 3),
		_cameraHeight * 15,
		cameraDistance * std::sinf(-XM_PIDIV2 + _cameraRotationFactor * 3),
		1.0f };

	_camera->setEyePosition(newEyePosition);
}

void pa::MyApplication::initializeD3dDevices(HWND hWnd)
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
