#pragma once
#include <vector>

#include "derivative_explorer/derivative_methods.hpp"
#include "derivative_explorer/function.hpp"
#include "derivative_explorer/result.hpp"

namespace de {

struct ComparisonConfig {
    const Function* function;
    std::vector<double> xs;
    std::vector<double> hs;
    std::vector<Method> methods;
};

// Order: for each x, for each h, for each method. Every method sees identical (x, h).
std::vector<DerivativeResult> run_comparison(const ComparisonConfig& cfg);

}  // namespace de
