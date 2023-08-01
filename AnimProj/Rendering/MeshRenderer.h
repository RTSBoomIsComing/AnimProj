// author: Changwan Yu
// not use now
// will be needed after complete refactoring
#pragma once
namespace pa
{
	class Mesh;
	class MeshRenderer
	{
	public:
		MeshRenderer(const Mesh& mesh);
		~MeshRenderer();

	private:
		void updateInstanceCBuffer();
		void render(UINT InstanceCount, UINT StartInstanceLocation);

	private:
		UINT					_instanceCount;
		ComPtr<ID3D11Buffer>	_instanceWorldCBuffer;
	};
}

