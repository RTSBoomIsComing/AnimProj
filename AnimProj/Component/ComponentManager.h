// author: Changwan Yu
#pragma once
namespace pa
{
	class Actor;

	template<typename ComponentType>
	struct ComponentManager
	{
		std::vector<ComponentType>			components;
		std::vector <std::weak_ptr<Actor>>	owners;
	};
}
