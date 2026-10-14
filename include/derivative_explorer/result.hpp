#pragma once
#include <optional>
#include <string>

namespace de {

enum class Status { Ok, InvalidStep, InvalidDomain, NonFinite };

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
