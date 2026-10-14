#include "derivative_explorer/comparison.hpp"

#include <cmath>
#include <limits>

#include "derivative_explorer/metrics.hpp"

namespace de {

std::vector<DerivativeResult> run_comparison(const ComparisonConfig& cfg) {
    constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();
    std::vector<DerivativeResult> out;
    for (double x : cfg.xs) {
        const bool ref_ok = cfg.function->valid(x) && std::isfinite(cfg.function->derivative(x));
        const double reference = ref_ok ? cfg.function->derivative(x) : kNaN;
        for (double h : cfg.hs) {
            for (Method m : cfg.methods) {
                const Estimate e = estimate_derivative(m, *cfg.function, x, h);
                DerivativeResult r{cfg.function->id, method_id(m), x, h, e.value, reference, kNaN,
                                   std::nullopt,     e.status};
                if (e.status == Status::Ok) {
                    if (!ref_ok) {
                        r.status = Status::NonFinite;
                        r.estimate = kNaN;
                    } else {
                        const ErrorMetrics em = compute_error(e.value, reference);
                        r.absolute_error = em.absolute;
                        r.relative_error = em.relative;
                    }
                }
                out.push_back(r);
            }
        }
    }
    return out;
}

}  // namespace de
