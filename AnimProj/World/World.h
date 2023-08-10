// author: Changwan Yu
#pragma once
#include "../Component/Component.h"
#include "../Component/ComponentManager.h"
namespace pa
{
	class GridMap;
	class Actor;
	class SceneComponent;
	class BehaviorTreeComponent;
	class World final
	{
	public:
		World();
		~World();
	public:
		void startGame();
		void update(float deltaTime);

	public:
		inline std::shared_ptr<GridMap> getDefaultMap() { return _map; }

	public:
		template<typename ActorType>
		void createActor(Transform const& transform);

		template<typename ComponentType>
		std::shared_ptr<ComponentManager<ComponentType>>& getComponentManager();

		template<typename ComponentType>
		std::vector<ComponentType>& getComponents();

		template<typename ComponentType>
		std::vector<std::weak_ptr<Actor>>& getOwners();

		template<typename ComponentType>
		ComponentHandle<ComponentType>	createComponent(std::weak_ptr<Actor> owner);

		template<typename ComponentType>
		void destroyComponent(ComponentHandle<ComponentType> componentHandle);

		template<typename ComponentType>
		ComponentType& getComponent(ComponentHandle<ComponentType> handle);
		

	private:
		std::shared_ptr<GridMap>					_map;
		std::vector<std::shared_ptr<Actor>>			_actors;

		std::tuple<
			std::shared_ptr<ComponentManager<SceneComponent>>,
			std::shared_ptr<ComponentManager<BehaviorTreeComponent>>> _componentManagers;
	};

	template<typename ActorType>
	inline void World::createActor(Transform const& transform)
	{
		std::shared_ptr<ActorType> actor = std::make_shared<ActorType>();
		actor->initializeComponents(*this);
		_actors.push_back(std::move(actor));
	}

	template<typename ComponentType>
	inline std::shared_ptr<ComponentManager<ComponentType>>& World::getComponentManager()
	{
		
		return std::get<std::shared_ptr<ComponentManager<ComponentType>>>(_componentManagers);
	}

	template<typename ComponentType>
	inline std::vector<ComponentType>& World::getComponents()
	{
		return getComponentManager<ComponentType>()->components;
	}

	template<typename ComponentType>
	inline std::vector<std::weak_ptr<Actor>>& World::getOwners()
	{
		return getComponentManager<ComponentType>()->owners;
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
		owners.back().lock()->setComponentHandle(componentHandle);
		assert(components.size() == owners.size());

		std::iter_swap(components.begin() + componentHandle.id, components.end() - 1);
		std::iter_swap(owners.begin() + componentHandle.id, owners.end() - 1);

		// need to consider using resize() instead of pop_back()
		// components.resize(components.size() - 1);
		components.pop_back();
		owners.pop_back();
	}

	template<typename ComponentType>
	inline ComponentType& World::getComponent(ComponentHandle<ComponentType> handle)
	{
		std::vector<ComponentType>& components = this->getComponents<ComponentType>();
		return components[handle.id];
	}
}

