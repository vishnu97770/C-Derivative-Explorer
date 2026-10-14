#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "derivative_explorer/comparison.hpp"
#include "derivative_explorer/csv_writer.hpp"

namespace {

constexpr const char* kUsage =
    "Usage: derivative-explorer [options]\n"
    "  --list-functions          List available functions and exit\n"
    "  --function ID             Function id (see --list-functions)\n"
    "  --x LIST                  Comma-separated evaluation points\n"
    "  --method NAME|--methods L forward, central (comma-separated list allowed; default both)\n"
    "  --step LIST|--steps LIST  Comma-separated positive step sizes h\n"
    "  --log-steps A:B[:N]       Alternative to --steps: N log-spaced h from 10^-A to 10^-B\n"
    "                            (default N = B-A+1 i.e. one per decade; e.g. 1:16)\n"
    "  --csv PATH                Write results to a CSV file\n"
    "  --help                    Show this help\n";

std::vector<std::string> split(const std::string& s, char sep) {
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, sep)) out.push_back(item);
    return out;
}

bool parse_double(const std::string& s, double& v) {
    if (s.empty()) return false;
    char* end = nullptr;
    v = std::strtod(s.c_str(), &end);
    return end == s.c_str() + s.size();
}

bool parse_list(const std::string& s, std::vector<double>& out) {
    for (const auto& tok : split(s, ',')) {
        double v;
        if (!parse_double(tok, v) || !std::isfinite(v)) return false;
        out.push_back(v);
    }
    return !out.empty();
}

int usage_error(const std::string& msg) {
    std::cerr << "error: " << msg << "\nTry 'derivative-explorer --help'.\n";
    return 2;
}

}  // namespace

int main(int argc, char** argv) {
    std::string function_id, csv_path, methods_arg = "forward,central";
    std::vector<double> xs, hs;
    bool list = false;

    auto need = [&](int& i, const std::string& flag) -> const char* {
        if (i + 1 >= argc) {
            usage_error("missing value for " + flag);
            return nullptr;
        }
        return argv[++i];
    };

    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--help" || a == "-h") {
            std::cout << kUsage;
            return 0;
        } else if (a == "--list-functions") {
            list = true;
        } else if (a == "--function" || a == "--csv" || a == "--method" || a == "--methods") {
            const char* v = need(i, a);
            if (!v) return 2;
            (a == "--function" ? function_id : a == "--csv" ? csv_path : methods_arg) = v;
        } else if (a == "--x" || a == "--step" || a == "--steps") {
            const char* v = need(i, a);
            if (!v) return 2;
            if (!parse_list(v, a == "--x" ? xs : hs))
                return usage_error("bad number list for " + a);
        } else if (a == "--log-steps") {
            const char* v = need(i, a);
            if (!v) return 2;
            const auto p = split(v, ':');
            double lo, hi, n = 0;
            if (p.size() < 2 || p.size() > 3 || !parse_double(p[0], lo) ||
                !parse_double(p[1], hi) || hi < lo ||
                (p.size() == 3 && (!parse_double(p[2], n) || n < 1)))
                return usage_error("--log-steps expects A:B[:N] with B >= A");
            if (p.size() == 2) n = hi - lo + 1;
            const int count = static_cast<int>(n);
            for (int k = 0; k < count; ++k) {
                const double e = count == 1 ? lo : lo + (hi - lo) * k / (count - 1);
                hs.push_back(std::pow(10.0, -e));
            }
        } else {
            return usage_error("unknown option '" + a + "'");
        }
    }

    if (list) {
        for (const auto& f : de::function_registry())
            std::cout << f.id << "\t" << f.description << "\n";
        return 0;
    }

    if (function_id.empty()) return usage_error("--function is required");
    const de::Function* fn = de::find_function(function_id);
    if (!fn) return usage_error("unknown function '" + function_id + "' (see --list-functions)");
    if (xs.empty()) return usage_error("--x is required");
    if (hs.empty()) return usage_error("--step, --steps or --log-steps is required");

    std::vector<de::Method> methods;
    for (const auto& name : split(methods_arg, ',')) {
        auto m = de::parse_method(name);
        if (!m) return usage_error("unknown method '" + name + "' (forward, central)");
        methods.push_back(*m);
    }

    try {
        const auto results = de::run_comparison({fn, xs, hs, methods});

        std::printf("%-6s %-8s %-12s %-12s %-24s %-24s %-12s %s\n", "func", "method", "x", "h",
                    "estimate", "reference", "abs_error", "status");
        std::size_t not_ok = 0;
        for (const auto& r : results) {
            if (r.status != de::Status::Ok) ++not_ok;
            std::printf("%-6s %-8s %-12.6g %-12.6g %-24.17g %-24.17g %-12.4e %s\n",
                        r.function_id.c_str(), r.method_id.c_str(), r.x, r.h, r.estimate,
                        r.reference, r.absolute_error, de::to_string(r.status));
        }
        std::printf("%zu results, %zu with non-ok status\n", results.size(), not_ok);

        if (!csv_path.empty()) {
            const std::string err = de::write_csv_file(csv_path, results);
            if (!err.empty()) {
                std::cerr << "error: " << err << "\n";
                return 1;
            }
            std::printf("wrote %s\n", csv_path.c_str());
        }
    } catch (const std::exception& e) {
        std::cerr << "internal error: " << e.what() << "\n";
        return 70;
    }
    return 0;
}
