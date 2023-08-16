// author: Changwan Yu
#pragma once
namespace pa
{
	class SceneComponent
	{
	public:
		inline bool isCulled() const { return _isCulled; }
		inline void setIsCulled(bool isCulled) { _isCulled = isCulled; }

	public:
		DirectX::XMFLOAT3 position	 = {};
		DirectX::XMFLOAT3 eulerAngle = {};

	private:
		bool _isCulled = false;
	};
}
