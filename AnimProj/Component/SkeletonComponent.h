// author: Changwan Yu
#pragma once
namespace pa
{
	class Skeleton;
	class Mesh;
	class SkeletonComponent
	{
	public:
		SkeletonComponent(ID3D11Device* device, const Skeleton& skeleton);
		~SkeletonComponent();
	
	public:
		void update(ID3D11DeviceContext* deviceContext, const Transform* pose, DirectX::XMVECTOR const& worldPosition, DirectX::XMVECTOR const& QWorldRotation);
		void render(ID3D11DeviceContext* deviceContext, const Mesh* boneMesh, const Mesh* boneToBoneMesh);
	private:
		const Skeleton*						_skeleton				= nullptr;

		std::vector<DirectX::XMFLOAT4X4>	_boneGTs;
		std::vector<DirectX::XMFLOAT4X4>	_boneToBoneGTs;
		ComPtr<ID3D11Buffer>				_boneWorldCBuffer;
		ComPtr<ID3D11Buffer>				_boneToBoneWorldCBuffer;
	};
}

