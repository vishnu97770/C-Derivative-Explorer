// Timing harness for derivative methods. See docs/benchmark-methodology.md.
// Workload: N evaluation points, one derivative estimate per point per method.
// Timing is never used as a pass/fail condition anywhere in the project.
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

#include "derivative_explorer/derivative_methods.hpp"

namespace {

double quantile(std::vector<double> v, double q) {
    std::sort(v.begin(), v.end());
    const double pos = q * static_cast<double>(v.size() - 1);
    const std::size_t lo = static_cast<std::size_t>(pos);
    const std::size_t hi = std::min(lo + 1, v.size() - 1);
    return v[lo] + (v[hi] - v[lo]) * (pos - static_cast<double>(lo));
}

}  // namespace

int main(int argc, char** argv) {
    const std::size_t n_points = argc > 1 ? std::strtoul(argv[1], nullptr, 10) : 100000;
    const int reps = argc > 2 ? static_cast<int>(std::strtol(argv[2], nullptr, 10)) : 15;
    const int warmup = 3;
    const char* raw_path = argc > 3 ? argv[3] : nullptr;
    const double h = 1e-5;
    if (n_points == 0 || reps < 3) {
        std::fprintf(stderr, "usage: bench [n_points>0] [repetitions>=3] [raw_csv]\n");
        return 2;
    }

    std::vector<double> xs(n_points);
    for (std::size_t i = 0; i < n_points; ++i)
        xs[i] = 0.1 + 2.0 * static_cast<double>(i) / static_cast<double>(n_points);

    std::ofstream raw;
    if (raw_path) {
        raw.open(raw_path);
        if (!raw) {
            std::fprintf(stderr, "error: cannot open '%s'\n", raw_path);
            return 1;
        }
        raw << "benchmark_id,function_id,method_id,n_points,h,rep,seconds,checksum,failures\n";
    }

    std::printf("benchmark_id=estimate_throughput_v1 n_points=%zu h=%g reps=%d warmup=%d\n",
                n_points, h, reps, warmup);
    std::printf("%-6s %-8s %12s %12s %12s %12s\n", "func", "method", "median_ns/pt", "q1_ns/pt",
                "q3_ns/pt", "failures");

    for (const char* fid : {"x2", "sin", "exp"}) {
        const de::Function& f = *de::find_function(fid);
        for (de::Method m : {de::Method::Forward, de::Method::Central}) {
            std::vector<double> per_point_ns;
            std::size_t failures = 0;
            for (int r = -warmup; r < reps; ++r) {
                double checksum = 0.0;  // keeps the work observable to the optimizer
                failures = 0;
                const auto t0 = std::chrono::steady_clock::now();
                for (double x : xs) {
                    const de::Estimate e = de::estimate_derivative(m, f, x, h);
                    if (e.status == de::Status::Ok)
                        checksum += e.value;
                    else
                        ++failures;
                }
                const auto t1 = std::chrono::steady_clock::now();
                const double secs = std::chrono::duration<double>(t1 - t0).count();
                // Reading checksum through a volatile sink prevents dead-code elimination.
                volatile double sink = checksum;
                (void)sink;
                if (r >= 0) {
                    per_point_ns.push_back(secs * 1e9 / static_cast<double>(n_points));
                    if (raw)
                        raw << "estimate_throughput_v1," << fid << "," << de::method_id(m) << ","
                            << n_points << "," << h << "," << r << "," << secs << "," << checksum
                            << "," << failures << "\n";
                }
            }
            std::printf("%-6s %-8s %12.2f %12.2f %12.2f %12zu\n", fid, de::method_id(m),
                        quantile(per_point_ns, 0.5), quantile(per_point_ns, 0.25),
                        quantile(per_point_ns, 0.75), failures);
        }
    }
    return 0;
}
