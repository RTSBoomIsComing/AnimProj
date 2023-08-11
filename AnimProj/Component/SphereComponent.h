// author: Changwan Yu
#pragma once

namespace pa
{
	class World;
	class Actor;
	class SphereComponent
	{
	public:
		SphereComponent() = default;

	public:
		void onUpdate(World& world, std::weak_ptr<Actor> owner, float deltaTime);


	public:
		inline float	getRadius() const { return _radius; }
		inline void		setRadius(float radius) { _radius = radius; }

	private:
		float _radius = 20.0f;
	};
}


