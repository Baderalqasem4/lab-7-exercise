#include <iostream>
#include "registry.h"

#include "entity_manager.h"

int main() {
    // test here...

    Registry<int> registry;

    // test storing and accessing objects
    size_t first = registry.store(15);
    size_t second = registry.store(30);

    assert(first == 0);
    assert(second == 1);
    assert(registry[first] == 15);
    assert(registry[second] == 30);
    assert(registry.size() == 2);

    // test removing an object
    registry.remove(first);
    assert(!registry.valid(first));
    assert(registry.size() == 1);

    // test reusing a removed ticket
    size_t third = registry.store(45);
    assert(third == first);
    assert(registry[third] == 45);
    assert(registry.valid(third));

    std::cout << "registry tests passed\n";

    // test entity creation
    EntityManager<int, float, char> manager;

    EntityID entity1 = manager.createEntity();
    EntityID entity2 = manager.createEntity();

    assert(entity1 == 0);
    assert(entity2 == 1);
    assert(manager.validEntity(entity1));
    assert(!manager.validEntity(10));

    // test adding components
    manager.addComponent(entity1, 15);
    manager.addComponent(entity1, 3.5f);
    manager.addComponent(entity2, 'a');

    // test checking components
    assert(manager.hasComponent<int>(entity1));
    assert(manager.hasComponent<float>(entity1));
    assert(!manager.hasComponent<char>(entity1));
    assert(manager.hasComponent<char>(entity2));

    // test getting components
    assert(manager.getComponent<int>(entity1) == 15);
    assert(manager.getComponent<float>(entity1) == 3.5f);
    assert(manager.getComponent<char>(entity2) == 'a');

    // test adding an existing variable
    int health = 80;
    manager.addComponent(entity2, health);
    assert(manager.getComponent<int>(entity2) == 80);

    std::cout << "entity manager tests passed\n";

    return 0;
}
