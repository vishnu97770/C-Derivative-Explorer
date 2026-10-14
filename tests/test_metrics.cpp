#include <gtest/gtest.h>

#include <cmath>
#include <limits>

#include "derivative_explorer/metrics.hpp"

TEST(Metrics, AbsoluteAndRelative) {
    const auto m = de::compute_error(6.01, 6.0);
    EXPECT_NEAR(m.absolute, 0.01, 1e-12);
    ASSERT_TRUE(m.relative.has_value());
    EXPECT_NEAR(*m.relative, 0.01 / 6.0, 1e-12);
}

TEST(Metrics, EqualValues) {
    const auto m = de::compute_error(2.5, 2.5);
    EXPECT_EQ(m.absolute, 0.0);
    EXPECT_EQ(m.relative, 0.0);
}

TEST(Metrics, ZeroReferenceHasNoRelativeError) {
    const auto m = de::compute_error(1e-3, 0.0);
    EXPECT_EQ(m.absolute, 1e-3);
    EXPECT_FALSE(m.relative.has_value());
}

TEST(Metrics, NearZeroReferenceHasNoRelativeError) {
    EXPECT_FALSE(de::compute_error(1.0, 1e-15).relative.has_value());
}

TEST(Metrics, NonFiniteInputs) {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();
    EXPECT_TRUE(std::isnan(de::compute_error(nan, 1.0).absolute));
    EXPECT_TRUE(std::isnan(de::compute_error(1.0, inf).absolute));
    EXPECT_FALSE(de::compute_error(nan, 1.0).relative.has_value());
}
