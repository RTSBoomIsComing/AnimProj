// author: Changwan Yu
#pragma once
namespace pa
{
	class GameObject;

	template <typename T>
	class GameComponent
	{
	protected:
		GameComponent()		= default;
		~GameComponent()	= default;

	private:
		static std::vector<T>			_components;
		static std::vector<GameObject*>	_owners;

		inline static GameObject*	findOwner(size_t ID);
		inline static size_t		create();
	};
}


