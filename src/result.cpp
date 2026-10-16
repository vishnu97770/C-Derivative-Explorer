#include "derivative_explorer/result.hpp"

namespace de {

const char* to_string(Status s) {
    switch (s) {
        case Status::Ok:
            return "ok";
        case Status::InvalidStep:
            return "invalid_step";
        case Status::InvalidDomain:
            return "invalid_domain";
        case Status::NonFinite:
            return "non_finite";
        case Status::DegenerateStep:
            return "degenerate_step";
    }
    return "unknown";
}

}  // namespace de
