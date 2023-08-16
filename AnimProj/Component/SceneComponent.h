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

		inline bool isPendingKill() const { return _isPendingKill; }


	public:
		DirectX::XMFLOAT3 position	 = {};
		DirectX::XMFLOAT3 eulerAngle = {};

	private:
		float _killTimer = 0.0f;
		bool _isCulled = false;
		bool _isPendingKill = false;
	};
}
