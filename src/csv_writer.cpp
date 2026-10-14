#include "derivative_explorer/csv_writer.hpp"

#include <cmath>
#include <cstdio>
#include <fstream>

namespace de {

const char* const kCsvHeader =
    "schema_version,function_id,method_id,x,h,estimate,reference,absolute_error,relative_error,"
    "status";

std::string csv_escape(const std::string& field) {
    if (field.find_first_of(",\"\n\r") == std::string::npos) return field;
    std::string out = "\"";
    for (char c : field) {
        if (c == '"') out += '"';
        out += c;
    }
    return out + "\"";
}

namespace {
std::string num(double v) {
    if (!std::isfinite(v)) return "";
    char buf[40];
    std::snprintf(buf, sizeof buf, "%.17g", v);
    return buf;
}
}  // namespace

void write_csv(std::ostream& os, const std::vector<DerivativeResult>& results) {
    os << kCsvHeader << '\n';
    for (const auto& r : results) {
        os << kCsvSchemaVersion << ',' << csv_escape(r.function_id) << ','
           << csv_escape(r.method_id) << ',' << num(r.x) << ',' << num(r.h) << ','
           << num(r.estimate) << ',' << num(r.reference) << ',' << num(r.absolute_error) << ','
           << (r.relative_error ? num(*r.relative_error) : std::string()) << ','
           << to_string(r.status) << '\n';
    }
}

std::string write_csv_file(const std::string& path, const std::vector<DerivativeResult>& results) {
    std::ofstream f(path);
    if (!f) return "cannot open '" + path + "' for writing";
    write_csv(f, results);
    f.flush();
    if (!f) return "write failed for '" + path + "'";
    f.close();
    if (f.fail()) return "close failed for '" + path + "'";
    return "";
}

}  // namespace de
