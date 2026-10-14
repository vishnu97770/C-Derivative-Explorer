#pragma once
#include <optional>

namespace de {

// |reference| below this is treated as zero for relative error purposes.
constexpr double kRelativeErrorReferenceFloor = 1e-12;

struct ErrorMetrics {
    double absolute;
    std::optional<double> relative;  // nullopt when undefined (reference ~ 0) or inputs non-finite
};

ErrorMetrics compute_error(double estimate, double reference);

}  // namespace de
