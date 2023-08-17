// author: Changwan Yu
#pragma once
namespace pa
{
	class World;
	class Actor;
	class SceneComponent
	{
	public:
		void onUpdate(World& world, Actor& owner, float deltaTime);
		
		inline bool isCulled() const { return _isCulled; }
		inline void setIsCulled(bool isCulled) { _isCulled = isCulled; }
	public:
		DirectX::XMFLOAT3 position	 = {};
		DirectX::XMFLOAT3 eulerAngle = {};

	private:
		bool _isCulled = false;
	};
}
