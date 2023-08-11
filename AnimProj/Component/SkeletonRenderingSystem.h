// author: Changwan Yu
#pragma once

namespace pa
{
	class SkeletonComponent;
	class Mesh;
	class SkeletonRenderingSystem
	{
	public:
		SkeletonRenderingSystem(ID3D11Device* device);
		~SkeletonRenderingSystem();
		SkeletonRenderingSystem(SkeletonRenderingSystem const&)	= delete;
		SkeletonRenderingSystem(SkeletonRenderingSystem&&)		= delete;

	public:
		void update(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		void render(ID3D11DeviceContext* deviceContext, const Mesh* boneMesh, const Mesh* boneToBoneMesh);

	private:
		ComPtr<ID3D11Buffer> _boneWorldSBuffer;
		ComPtr<ID3D11ShaderResourceView> _SRVBoneWorld;

		ComPtr<ID3D11Buffer> _boneToBoneWorldSBuffer;
		ComPtr<ID3D11ShaderResourceView> _SRVBoneToBoneWorld;

	private:
	};
}

