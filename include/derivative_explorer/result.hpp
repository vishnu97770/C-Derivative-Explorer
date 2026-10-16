#pragma once
#include <optional>
#include <string>

namespace de {

// DegenerateStep: h > 0 but a stencil point rounds to the base point (x + h == x), so the
// formula would divide by h while using f(x) twice. Distinct from InvalidStep (bad input h).
enum class Status { Ok, InvalidStep, InvalidDomain, NonFinite, DegenerateStep };

const char* to_string(Status s);

struct DerivativeResult {
    std::string function_id;
    std::string method_id;
    double x;
    double h;
    double estimate;        // NaN unless status == Ok
    double reference;       // NaN if non-finite
    double absolute_error;  // NaN unless status == Ok
    std::optional<double> relative_error;
    Status status;
};

}  // namespace de
