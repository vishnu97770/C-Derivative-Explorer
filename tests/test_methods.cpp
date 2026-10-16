#include <gtest/gtest.h>

#include <cmath>
#include <limits>

#include "derivative_explorer/derivative_methods.hpp"
#include "derivative_explorer/metrics.hpp"

using de::Status;

// Tolerance rationale: for x^2 at x=3, h=0.01 the values involve ~9 so roundoff in the
// difference is ~1e-15, divided by h gives ~1e-13. 1e-9 is generous but still far below
// the 0.01 truncation error that distinguishes forward from central.
TEST(Methods, ForwardQuadratic) {
    const auto e = de::forward_difference(*de::find_function("x2"), 3.0, 0.01);
    EXPECT_EQ(e.status, Status::Ok);
    EXPECT_NEAR(e.value, 6.01, 1e-9);
}

TEST(Methods, CentralQuadraticExactInTheory) {
    const auto e = de::central_difference(*de::find_function("x2"), 3.0, 0.01);
    EXPECT_EQ(e.status, Status::Ok);
    EXPECT_NEAR(e.value, 6.0, 1e-9);
}

TEST(Methods, InvalidStepRejected) {
    const auto& f = *de::find_function("x2");
    const double bad[] = {0.0, -1.0, std::numeric_limits<double>::quiet_NaN(),
                          std::numeric_limits<double>::infinity(),
                          -std::numeric_limits<double>::infinity()};
    for (double h : bad) {
        EXPECT_EQ(de::forward_difference(f, 3.0, h).status, Status::InvalidStep) << h;
        EXPECT_EQ(de::central_difference(f, 3.0, h).status, Status::InvalidStep) << h;
        EXPECT_TRUE(std::isnan(de::central_difference(f, 3.0, h).value));
    }
}

TEST(Methods, DomainEdgeNoFallback) {
    const auto& f = *de::find_function("log");
    // x=0.005 valid, but x-h = -0.005 is not: central must fail, forward must still work.
    EXPECT_EQ(de::central_difference(f, 0.005, 0.01).status, Status::InvalidDomain);
    EXPECT_EQ(de::forward_difference(f, 0.005, 0.01).status, Status::Ok);
    EXPECT_EQ(de::forward_difference(f, 0.0, 0.01).status, Status::InvalidDomain);
}

TEST(Methods, ExpOverflowIsNonFinite) {
    const auto& f = *de::find_function("exp");
    EXPECT_EQ(de::central_difference(f, 1000.0, 0.01).status, Status::NonFinite);
    EXPECT_EQ(de::forward_difference(f, 1000.0, 0.01).status, Status::NonFinite);
}

// Observed-order checks, only in the truncation-dominated range (h >= 1e-4 for sin at x=1).
// Central error ~ C h^2: shrinking h by 10 shrinks error by ~100 (accept > 50).
// Forward error ~ C h:   shrinking h by 10 shrinks error by ~10  (accept > 5).
TEST(Methods, ObservedConvergenceOrder) {
    const auto& f = *de::find_function("sin");
    const double ref = f.derivative(1.0);
    auto err = [&](de::Method m, double h) {
        return de::compute_error(de::estimate_derivative(m, f, 1.0, h).value, ref).absolute;
    };
    for (double h : {1e-1, 1e-2, 1e-3}) {
        EXPECT_GT(err(de::Method::Central, h) / err(de::Method::Central, h / 10), 50.0) << h;
        EXPECT_GT(err(de::Method::Forward, h) / err(de::Method::Forward, h / 10), 5.0) << h;
    }
}

TEST(Methods, ParseMethod) {
    EXPECT_EQ(de::parse_method("forward"), de::Method::Forward);
    EXPECT_EQ(de::parse_method("central"), de::Method::Central);
    EXPECT_FALSE(de::parse_method("backward").has_value());
}

// Degenerate stencils: h > 0 is valid input, but a stencil point rounds to x. The test points
// follow from IEEE double spacing: ulp(1) = epsilon = 2.22e-16, and x + h rounds to x when
// h < epsilon/2 (ties round to even, so h = epsilon/2 also rounds back to 1).
TEST(Methods, DegenerateForward) {
    const auto& f = *de::find_function("sin");
    for (double h : {1e-17, 1e-16, std::numeric_limits<double>::epsilon() / 2}) {
        const auto e = de::forward_difference(f, 1.0, h);
        EXPECT_EQ(e.status, Status::DegenerateStep) << h;
        EXPECT_TRUE(std::isnan(e.value)) << h;
    }
}

TEST(Methods, DegenerateCentral) {
    const auto& f = *de::find_function("sin");
    for (double h : {1e-17, 1e-16}) {
        const auto e = de::central_difference(f, 1.0, h);
        EXPECT_EQ(e.status, Status::DegenerateStep) << h;
        EXPECT_TRUE(std::isnan(e.value)) << h;
    }
}

// At x = 1, h = 1e-16: x + h rounds to 1 but x - h = 0.99999999999999989 is representable.
// Previously this one-sided rounding produced an "ok" estimate (~0.555 for sin'(1) = 0.540).
TEST(Methods, CentralOneSidedRoundingIsDegenerate) {
    const double h = 1e-16;
    ASSERT_EQ(1.0 + h, 1.0);
    ASSERT_NE(1.0 - h, 1.0);
    EXPECT_EQ(de::central_difference(*de::find_function("sin"), 1.0, h).status,
              Status::DegenerateStep);
}

TEST(Methods, SmallestUsableStepIsStillOk) {
    const auto& f = *de::find_function("sin");
    const double eps = std::numeric_limits<double>::epsilon();
    EXPECT_EQ(de::forward_difference(f, 1.0, eps).status, Status::Ok);
    EXPECT_EQ(de::central_difference(f, 1.0, eps).status, Status::Ok);
}

TEST(Methods, DegenerateDependsOnMagnitudeOfX) {
    const auto& f = *de::find_function("x2");
    // ulp(1e16) = 2, so x + 1 rounds back to x (tie to even), yet the same h is fine near 1.
    EXPECT_EQ(de::forward_difference(f, 1e16, 1.0).status, Status::DegenerateStep);
    EXPECT_EQ(de::forward_difference(f, 1.0, 1.0).status, Status::Ok);
    // Near zero every positive h gives distinct points.
    EXPECT_EQ(de::forward_difference(f, 0.0, 1e-300).status, Status::Ok);
}

TEST(Methods, DomainAndStepErrorsTakePrecedenceOverDegenerate) {
    const auto& lg = *de::find_function("log");
    // x = -1 is outside the domain of log; 1e-20 would also be degenerate there.
    EXPECT_EQ(de::forward_difference(lg, -1.0, 1e-20).status, Status::InvalidDomain);
    EXPECT_EQ(de::central_difference(lg, -1.0, 1e-20).status, Status::InvalidDomain);
    EXPECT_EQ(de::forward_difference(lg, 1.0, 0.0).status, Status::InvalidStep);
    const double inf = std::numeric_limits<double>::infinity();
    EXPECT_EQ(de::forward_difference(*de::find_function("x2"), inf, 0.1).status,
              Status::InvalidDomain);
}
