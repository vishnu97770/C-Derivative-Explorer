#pragma once
#include <optional>
#include <string>

#include "derivative_explorer/function.hpp"
#include "derivative_explorer/result.hpp"

namespace de {

enum class Method { Forward, Central };

const char* method_id(Method m);
std::optional<Method> parse_method(const std::string& s);

struct Estimate {
    double value;  // NaN unless status == Ok
    Status status;
};

// h must be finite and > 0. All stencil points must be in the function's domain.
// No silent fallback and no silent alteration of h.
Estimate forward_difference(const Function& f, double x, double h);  // [f(x+h)-f(x)]/h
Estimate central_difference(const Function& f, double x, double h);  // [f(x+h)-f(x-h)]/(2h)
Estimate estimate_derivative(Method m, const Function& f, double x, double h);

}  // namespace de
