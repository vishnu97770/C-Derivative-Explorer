#include "derivative_explorer/function.hpp"

#include <cmath>

namespace de {

namespace {
bool all_reals(double) { return true; }

std::vector<Function> build() {
    return {
        {"x2", [](double x) { return x * x; }, [](double x) { return 2.0 * x; }, all_reals,
         "f(x)=x^2, f'(x)=2x"},
        {"x3", [](double x) { return x * x * x; }, [](double x) { return 3.0 * x * x; }, all_reals,
         "f(x)=x^3, f'(x)=3x^2"},
        {"sin", [](double x) { return std::sin(x); }, [](double x) { return std::cos(x); },
         all_reals, "f(x)=sin(x), f'(x)=cos(x); radians"},
        {"cos", [](double x) { return std::cos(x); }, [](double x) { return -std::sin(x); },
         all_reals, "f(x)=cos(x), f'(x)=-sin(x); radians"},
        {"exp", [](double x) { return std::exp(x); }, [](double x) { return std::exp(x); },
         all_reals, "f(x)=exp(x), f'(x)=exp(x); overflows for x > ~709.78"},
        {"log", [](double x) { return std::log(x); }, [](double x) { return 1.0 / x; },
         [](double x) { return std::isfinite(x) && x > 0.0; },
         "f(x)=ln(x), f'(x)=1/x; domain x > 0 (demonstrates domain-edge handling)"},
    };
}
}  // namespace

const std::vector<Function>& function_registry() {
    static const std::vector<Function> registry = build();
    return registry;
}

const Function* find_function(const std::string& id) {
    for (const auto& f : function_registry())
        if (f.id == id) return &f;
    return nullptr;
}

}  // namespace de
