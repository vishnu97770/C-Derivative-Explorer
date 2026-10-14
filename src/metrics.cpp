#include "derivative_explorer/metrics.hpp"

#include <cmath>
#include <limits>

namespace de {

ErrorMetrics compute_error(double estimate, double reference) {
    if (!std::isfinite(estimate) || !std::isfinite(reference))
        return {std::numeric_limits<double>::quiet_NaN(), std::nullopt};
    const double abs_err = std::fabs(estimate - reference);
    if (std::fabs(reference) < kRelativeErrorReferenceFloor) return {abs_err, std::nullopt};
    return {abs_err, abs_err / std::fabs(reference)};
}

}  // namespace de
