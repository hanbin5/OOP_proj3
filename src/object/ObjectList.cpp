#pragma once

#include <algorithm>
#include <list>
#include <memory>
#include <utility>

#include "Object.cpp"
#include "ObjectSpec.cpp"

class ObjectList {
private:
    std::list<std::unique_ptr<Object>> objects;
public:
    void push(std::unique_ptr<Object> obj) {
        objects.push_back(std::move(obj));
    }

    auto begin() { return objects.begin(); }
    auto end() { return objects.end(); }
    auto begin() const { return objects.begin(); }
    auto end() const { return objects.end(); }
    auto cbegin() const { return objects.cbegin(); }
    auto cend() const { return objects.cend(); }

    void reset() {
        objects.clear();
    }

    size_t size() const {
        return objects.size();
    }
};
