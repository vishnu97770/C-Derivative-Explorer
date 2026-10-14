#pragma once
#include <ostream>
#include <string>
#include <vector>

#include "derivative_explorer/result.hpp"

namespace de {

constexpr int kCsvSchemaVersion = 1;
extern const char* const kCsvHeader;

std::string csv_escape(const std::string& field);

// Non-finite numbers are written as empty fields; the status column explains why.
// Numbers use %.17g (round-trip precision, '.' decimal separator, no locale).
void write_csv(std::ostream& os, const std::vector<DerivativeResult>& results);

// Returns empty string on success, otherwise a message including the path.
std::string write_csv_file(const std::string& path, const std::vector<DerivativeResult>& results);

}  // namespace de
