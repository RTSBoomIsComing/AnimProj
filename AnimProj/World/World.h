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
		World()		= default;
		~World()	= default;
	public:
		bool setDefaultMap(std::shared_ptr<Map> map);

		template<typename ActorType>
		bool spawnActor(Transform const& transform);

		template<typename ComponentType>
		std::vector<ComponentType>& getComponents();

		template<typename ComponentType>
		ComponentHandle<ComponentType> createComponent();

	private:
		std::shared_ptr<Map>					_map;
		std::vector<std::shared_ptr<Actor>>		_acters;

		std::tuple<ComponentManager<SceneComponent>> _componentManagers;
	};
}

template<typename ActorType>
inline bool pa::World::spawnActor(Transform const& transform)
{
	std::shared_ptr<ActorType> spawnedActor = std::make_shared<ActorType>();
	spawnedActor->initializeComponents(*this);
	_acters.push_back(std::move(spawnedActor));

	return true;
}

template<typename ComponentType>
inline std::vector<ComponentType>& pa::World::getComponents()
{
	return std::get<ComponentManager<SceneComponent>>(_componentManagers).components;
}

template<typename ComponentType>
inline pa::ComponentHandle<ComponentType> pa::World::createComponent() 
{
	std::vector<ComponentType>& components = this->getComponents<ComponentType>();
	ComponentHandle<ComponentType> handle(components, components.size());
	components.emplace_back();
	return handle;
}

