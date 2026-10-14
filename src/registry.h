#ifndef REGISTRY_H
#define REGISTRY_H

#include <utility>
#include <cassert>
#include <vector>
#include <cstddef>
#include <deque>

template <typename T>
class Registry {
    private:
        // store the objects
        std::vector<T> objects;

        // track which tickets are active
        std::vector<bool> active;

        // store removed tickets to reuse later
        std::vector<size_t> freeIds;

        // track the number of active objects
        size_t activeObj = 0;

    public:
        // copy an object into the registry
        size_t store(const T& object) {
        size_t id;

        if (!freeIds.empty()) {
            id = freeIds.back();
            freeIds.pop_back();
            objects[id] = object;
            active[id] = true;
        } 
        
        else {
            id = objects.size();
            objects.push_back(object);
            active.push_back(true);
        }

        activeObj++;
        return id;
    }

    // move an object into the registry
    size_t store(T&& object) {
        size_t id;

        if (!freeIds.empty()) {
            id = freeIds.back();
            freeIds.pop_back();
            objects[id] = std::move(object);
            active[id] = true;
        } 
        
        else {
            id = objects.size();
            objects.push_back(std::move(object));
            active.push_back(true);
        }

        activeObj++;
        return id;
    }

    // remove an object and recycle its ticket
    void remove(size_t id) {
        assert(valid(id));

        objects[id] = T{};
        active[id] = false;
        freeIds.push_back(id);
        activeObj--;
    }

    // check if a ticket exists and is active
    bool valid(size_t id) const {
        return id < active.size() && active[id];
    }

    // return a reference to the stored object
    T& operator[](size_t id) {
        assert(valid(id));
        return objects[id];
    }

    // return a const reference for a const registry
    const T& operator[](size_t id) const {
        assert(valid(id));
        return objects[id];
    }

    // return the number of active objects
    size_t size() const {
        return activeObj;
    }

};


#endif  // REGISTRY_H
