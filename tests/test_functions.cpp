#include <gtest/gtest.h>

#include <cmath>

#include "derivative_explorer/function.hpp"

TEST(Functions, RegistryLookup) {
    EXPECT_NE(de::find_function("x2"), nullptr);
    EXPECT_EQ(de::find_function("nope"), nullptr);
}

TEST(Functions, QuadraticReferenceExact) {
    const auto* f = de::find_function("x2");
    EXPECT_EQ(f->value(3.0), 9.0);
    EXPECT_EQ(f->derivative(3.0), 6.0);  // exactly representable
}

TEST(Functions, KnownValues) {
    EXPECT_DOUBLE_EQ(de::find_function("x3")->derivative(2.0), 12.0);
    EXPECT_DOUBLE_EQ(de::find_function("sin")->derivative(0.0), 1.0);
    EXPECT_DOUBLE_EQ(de::find_function("cos")->derivative(0.0), -0.0);
    EXPECT_DOUBLE_EQ(de::find_function("exp")->derivative(0.0), 1.0);
    EXPECT_DOUBLE_EQ(de::find_function("log")->derivative(2.0), 0.5);
}

TEST(Functions, RadiansNotDegrees) {
    EXPECT_NEAR(de::find_function("sin")->value(M_PI / 2), 1.0, 1e-15);
}

TEST(Functions, DomainPredicates) {
    EXPECT_TRUE(de::find_function("x2")->valid(-5.0));
    EXPECT_FALSE(de::find_function("log")->valid(0.0));
    EXPECT_FALSE(de::find_function("log")->valid(-1.0));
    EXPECT_TRUE(de::find_function("log")->valid(1.0));
}

TEST(Functions, ExpOverflowIsNonFinite) {
    EXPECT_FALSE(std::isfinite(de::find_function("exp")->value(1000.0)));
}
