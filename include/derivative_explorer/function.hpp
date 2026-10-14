#pragma once
#include <functional>
#include <string>
#include <vector>

namespace de {

// A test function with a known analytical derivative. Angles are in radians.
struct Function {
    std::string id;
    std::function<double(double)> value;
    std::function<double(double)> derivative;  // analytical reference
    std::function<bool(double)> valid;         // explicit domain predicate
    std::string description;
};

const std::vector<Function>& function_registry();
const Function* find_function(const std::string& id);

}  // namespace de
