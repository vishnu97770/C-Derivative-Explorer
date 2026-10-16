#include <gtest/gtest.h>

#include <cmath>

#include "derivative_explorer/comparison.hpp"

TEST(Comparison, IdenticalInputsAcrossMethodsAndStableOrder) {
    const auto* f = de::find_function("sin");
    const auto rs = de::run_comparison(
        {f, {0.5, 1.0}, {0.1, 0.01}, {de::Method::Forward, de::Method::Central}});
    ASSERT_EQ(rs.size(), 8u);
    // order: x, then h, then method
    EXPECT_EQ(rs[0].method_id, "forward");
    EXPECT_EQ(rs[1].method_id, "central");
    for (std::size_t i = 0; i < rs.size(); i += 2) {
        EXPECT_EQ(rs[i].x, rs[i + 1].x);
        EXPECT_EQ(rs[i].h, rs[i + 1].h);
        EXPECT_EQ(rs[i].reference, rs[i + 1].reference);
        EXPECT_EQ(rs[i].function_id, "sin");
    }
    EXPECT_EQ(rs[0].x, 0.5);
    EXPECT_EQ(rs[0].h, 0.1);
    EXPECT_EQ(rs[2].h, 0.01);
    EXPECT_EQ(rs[4].x, 1.0);
}

TEST(Comparison, FailuresAreExplicitRecords) {
    const auto* f = de::find_function("log");
    const auto rs =
        de::run_comparison({f, {0.005}, {0.01, -1.0}, {de::Method::Forward, de::Method::Central}});
    ASSERT_EQ(rs.size(), 4u);
    EXPECT_EQ(rs[0].status, de::Status::Ok);             // forward h=0.01
    EXPECT_EQ(rs[1].status, de::Status::InvalidDomain);  // central h=0.01
    EXPECT_EQ(rs[2].status, de::Status::InvalidStep);
    EXPECT_EQ(rs[3].status, de::Status::InvalidStep);
    EXPECT_TRUE(std::isnan(rs[1].estimate));
    EXPECT_TRUE(std::isnan(rs[1].absolute_error));
}

TEST(Comparison, ZeroReferenceRecord) {
    const auto rs =
        de::run_comparison({de::find_function("x2"), {0.0}, {0.01}, {de::Method::Forward}});
    ASSERT_EQ(rs.size(), 1u);
    EXPECT_EQ(rs[0].status, de::Status::Ok);
    EXPECT_FALSE(rs[0].relative_error.has_value());
    EXPECT_NEAR(rs[0].absolute_error, 0.01, 1e-12);
}

TEST(Comparison, OverflowVisible) {
    const auto rs =
        de::run_comparison({de::find_function("exp"), {1000.0}, {0.01}, {de::Method::Central}});
    EXPECT_EQ(rs[0].status, de::Status::NonFinite);
}

TEST(Comparison, DegenerateStepRecordHasNoMisleadingMetrics) {
    const auto rs = de::run_comparison(
        {de::find_function("sin"), {1.0}, {1e-17}, {de::Method::Forward, de::Method::Central}});
    ASSERT_EQ(rs.size(), 2u);
    for (const auto& r : rs) {
        EXPECT_EQ(r.status, de::Status::DegenerateStep);
        EXPECT_TRUE(std::isnan(r.estimate));
        EXPECT_TRUE(std::isnan(r.absolute_error));
        EXPECT_FALSE(r.relative_error.has_value());
        EXPECT_NEAR(r.reference, std::cos(1.0), 1e-15);  // reference is still reported
    }
    EXPECT_STREQ(de::to_string(de::Status::DegenerateStep), "degenerate_step");
}
