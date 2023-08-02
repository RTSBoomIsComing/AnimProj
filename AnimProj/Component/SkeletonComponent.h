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
		~SkeletonComponent() = default;

		SkeletonComponent(SkeletonComponent&& other) = default;
	
	public:
		void update(const Transform* pose, DirectX::XMFLOAT3 const& worldPosition, DirectX::XMFLOAT4 const& worldRotation);
		void render(ID3D11DeviceContext* deviceContext, const Mesh* boneMesh, const Mesh* boneToBoneMesh);
	private:
		const Skeleton*						_skeleton				= nullptr;

		std::vector<DirectX::XMFLOAT4X4>	_boneGTs;
		std::vector<DirectX::XMFLOAT4X4>	_boneToBoneGTs;
		ComPtr<ID3D11Buffer>				_boneWorldCBuffer;
		ComPtr<ID3D11Buffer>				_boneToBoneWorldCBuffer;
	};
}

