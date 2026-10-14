#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <cstddef>  // size_t
#include <array>
#include <vector>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

#include "pack.h"  // Pack from prep
#include "registry.h"


using EntityID = size_t;

template <typename... ComponentTypes>
class EntityManager {

    private:
    // store a registry for each component type
    std::tuple<Registry<ComponentTypes>...> registries;

    // store component indices for each entity
    std::vector<std::array<size_t, sizeof...(ComponentTypes)>> componentIndices;

    // track which entities are active
    std::vector<bool> activeEntities;

    // store removed entity ids for reuse
    std::vector<EntityID> freeIds;

    // track the next new entity id
    EntityID nextId = 0;

    // represent an invalid component index
    static constexpr size_t INVALID_INDEX = std::numeric_limits<size_t>::max();

    public:

    // create an entity and return its id
    EntityID createEntity() {
        EntityID id;

        // reuse a removed id if available
        if (!freeIds.empty()) {
            id = freeIds.back();
            freeIds.pop_back();
        } 
        
        else {
            // create a new entity id
            id = nextId;
            nextId++;

            activeEntities.push_back(false);
            componentIndices.push_back({});
        }

        // mark the entity as active
        activeEntities[id] = true;

        // initialize all component indices as invalid
        componentIndices[id].fill(INVALID_INDEX);

        return id;
    }

    // check if an entity exists and is active
    bool validEntity(EntityID id) const {
        return id < activeEntities.size() && activeEntities[id];
    }

    // check if an entity has a specific component
    template <typename ComponentType>
    bool hasComponent(EntityID id) const {
        assert(validEntity(id));

        constexpr int typeIndex = Pack<ComponentTypes...>::template index<ComponentType>;

        static_assert(typeIndex != -1, "invalid component type");

        return componentIndices[id][typeIndex] != INVALID_INDEX;
    }

    // return a reference to an entity's component
    template <typename ComponentType>
    ComponentType& getComponent(EntityID id) {
        assert(validEntity(id));
        assert(hasComponent<ComponentType>(id));

        // find the component type index
        constexpr int typeIndex = Pack<ComponentTypes...>::template index<ComponentType>;

        // get the stored component index
        size_t componentIndex = componentIndices[id][typeIndex];

        // return the component from its registry
        return std::get<typeIndex>(registries)[componentIndex];
    }

    // add a component to an entity
    template <typename ComponentType>
    void addComponent(EntityID id, ComponentType&& component) {
        assert(validEntity(id));

        // remove the reference from the component type
        using Type = std::remove_reference_t<ComponentType>;

        // make sure the entity does not already have this component
        assert(!hasComponent<Type>(id));

        // find the component type index
        constexpr int typeIndex = Pack<ComponentTypes...>::template index<Type>;

        // store the component in the correct registry
        size_t componentIndex = std::get<typeIndex>(registries).store(std::forward<ComponentType>(component));

        // save the components ticket
        componentIndices[id][typeIndex] = componentIndex;
    }

};

#endif  // ENTITY_MANAGER_H
