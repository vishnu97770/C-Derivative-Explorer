#include "derivative_explorer/derivative_methods.hpp"

#include <cmath>
#include <limits>

namespace de {

namespace {
constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();

bool valid_step(double h) { return std::isfinite(h) && h > 0.0; }

// Evaluates f at pt; reports domain/non-finite problems via status.
bool eval(const Function& f, double pt, double& out, Status& status) {
    if (!std::isfinite(pt) || !f.valid(pt)) {
        status = Status::InvalidDomain;
        return false;
    }
    out = f.value(pt);
    if (!std::isfinite(out)) {
        status = Status::NonFinite;
        return false;
    }
    return true;
}

Estimate fail(Status s) { return {kNaN, s}; }
}  // namespace

const char* method_id(Method m) { return m == Method::Forward ? "forward" : "central"; }

std::optional<Method> parse_method(const std::string& s) {
    if (s == "forward") return Method::Forward;
    if (s == "central") return Method::Central;
    return std::nullopt;
}

Estimate forward_difference(const Function& f, double x, double h) {
    if (!valid_step(h)) return fail(Status::InvalidStep);
    Status st = Status::Ok;
    double f0 = 0, f1 = 0;
    if (!eval(f, x, f0, st) || !eval(f, x + h, f1, st)) return fail(st);
    const double d = (f1 - f0) / h;
    return std::isfinite(d) ? Estimate{d, Status::Ok} : fail(Status::NonFinite);
}

Estimate central_difference(const Function& f, double x, double h) {
    if (!valid_step(h)) return fail(Status::InvalidStep);
    Status st = Status::Ok;
    double fm = 0, fp = 0;
    if (!eval(f, x - h, fm, st) || !eval(f, x + h, fp, st)) return fail(st);
    const double d = (fp - fm) / (2.0 * h);
    return std::isfinite(d) ? Estimate{d, Status::Ok} : fail(Status::NonFinite);
}

Estimate estimate_derivative(Method m, const Function& f, double x, double h) {
    return m == Method::Forward ? forward_difference(f, x, h) : central_difference(f, x, h);
}

}  // namespace de
