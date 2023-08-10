// author: Changwan Yu
#pragma once
#include "../Component/Component.h"
#include "../Component/ComponentManager.h"
namespace pa
{
	class Map;
	class Actor;
	class SceneComponent;
	class World final
	{
	public:
		World();
		~World();
	public:
		bool setDefaultMap(std::shared_ptr<Map> map);

		template<typename ActorType>
		bool spawnActor(Transform const& transform);

		template<typename ComponentType>
		std::vector<ComponentType>& getComponents();

		template<typename ComponentType>
		std::vector<std::weak_ptr<Actor>>& getOwners();

		template<typename ComponentType>
		ComponentHandle<ComponentType>	createComponent(std::weak_ptr<Actor> owner);

		template<typename ComponentType>
		void destroyComponent(ComponentHandle<ComponentType> componentHandle);
		

	private:
		std::shared_ptr<Map>						_map;
		std::vector<std::shared_ptr<Actor>>			_acters;

		std::tuple<std::shared_ptr<ComponentManager<SceneComponent>>> _componentManagers;
	};

	template<typename ActorType>
	inline bool World::spawnActor(Transform const& transform)
	{
		std::shared_ptr<ActorType> spawnedActor = std::make_shared<ActorType>();
		spawnedActor->initializeComponents(*this);
		_acters.push_back(std::move(spawnedActor));

		// add to map
		return true;
	}

	template<typename ComponentType>
	inline std::vector<ComponentType>& World::getComponents()
	{
		return std::get<std::shared_ptr<ComponentManager<ComponentType>>>(_componentManagers)->components;
	}

	template<typename ComponentType>
	inline std::vector<std::weak_ptr<Actor>>& World::getOwners()
	{
		return std::get<std::shared_ptr<ComponentManager<ComponentType>>>(_componentManagers)->owners;
	}

	template<typename ComponentType>
	inline ComponentHandle<ComponentType> World::createComponent(std::weak_ptr<Actor> owner) 
	{
		std::vector<ComponentType>&			components = this->getComponents<ComponentType>();
		std::vector<std::weak_ptr<Actor>>&	owners = this->getOwners<ComponentType>();
		
		assert(components.size() == owners.size());
		size_t newComponentID = components.size();
		ComponentHandle<ComponentType> handle(newComponentID);

		components.emplace_back();
		owners.push_back(owner);
		
		return handle;
	}

	template<typename ComponentType>
	inline void World::destroyComponent(ComponentHandle<ComponentType> componentHandle)
	{
		std::vector<ComponentType>& components = this->getComponents<ComponentType>();
		std::vector<std::weak_ptr<Actor>>& owners = this->getOwners<ComponentType>();

		assert(components.size() == owners.size());

		std::iter_swap(components.begin() + componentHandle.id, components.end() - 1);
		std::iter_swap(owners.begin() + componentHandle.id, owners.end() - 1);

		// need to consider using resize() instead of pop_back()
		// components.resize(components.size() - 1);
		components.pop_back();
		owners.pop_back();
	}
}

