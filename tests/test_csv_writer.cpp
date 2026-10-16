#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <sstream>

#include "derivative_explorer/comparison.hpp"
#include "derivative_explorer/csv_writer.hpp"

TEST(Csv, StableHeader) {
    std::ostringstream os;
    de::write_csv(os, {});
    EXPECT_EQ(os.str(),
              "schema_version,function_id,method_id,x,h,estimate,reference,absolute_error,"
              "relative_error,status\n");
}

TEST(Csv, EscapingCommasAndQuotes) {
    EXPECT_EQ(de::csv_escape("plain"), "plain");
    EXPECT_EQ(de::csv_escape("a,b"), "\"a,b\"");
    EXPECT_EQ(de::csv_escape("say \"hi\""), "\"say \"\"hi\"\"\"");
}

TEST(Csv, RowsAndEmptyFieldsForFailures) {
    const auto rs =
        de::run_comparison({de::find_function("x2"), {3.0}, {0.01, 0.0}, {de::Method::Forward}});
    std::ostringstream os;
    de::write_csv(os, rs);
    std::istringstream is(os.str());
    std::string header, row1, row2;
    std::getline(is, header);
    std::getline(is, row1);
    std::getline(is, row2);
    EXPECT_EQ(row1.rfind("1,x2,forward,3,0.01,", 0), 0u) << row1;
    EXPECT_NE(row1.find(",ok"), std::string::npos);
    EXPECT_EQ(row2, "1,x2,forward,3,0,,6,,,invalid_step");
    EXPECT_EQ(os.str().find("nan"), std::string::npos);
    EXPECT_EQ(os.str().find("inf"), std::string::npos);
}

TEST(Csv, FailedPathReportsError) {
    const std::string err = de::write_csv_file("/nonexistent_dir_xyz/out.csv", {});
    EXPECT_FALSE(err.empty());
    EXPECT_NE(err.find("/nonexistent_dir_xyz/out.csv"), std::string::npos);
}

TEST(Csv, WritesFile) {
    const std::string path = testing::TempDir() + "de_test.csv";
    EXPECT_EQ(de::write_csv_file(path, {}), "");
    std::ifstream f(path);
    std::string line;
    std::getline(f, line);
    EXPECT_EQ(line, de::kCsvHeader);
    std::remove(path.c_str());
}

TEST(Csv, DegenerateStepRow) {
    const auto rs = de::run_comparison(
        {de::find_function("x2"), {1.0}, {1e-17}, {de::Method::Forward}});
    std::ostringstream os;
    de::write_csv(os, rs);
    // estimate, absolute_error and relative_error are empty; the reason is in status.
    // h is written with %.17g, so the double nearest 1e-17 prints as 1.0000000000000001e-17.
    EXPECT_EQ(os.str(), std::string(de::kCsvHeader) +
                            "\n1,x2,forward,1,1.0000000000000001e-17,,2,,,degenerate_step\n");
}
