
#include "id_utils.h"
#include <random>
#include <cmath>
#include <cfenv>
#include <climits>

namespace bt {
    // TODO: Optim - probably not good enough to be used as system-wide unique id.
    // This needs to be changed in case we change id_t underlying type.
    static std::int64_t generate_random_int64() {
        static std::random_device rd;
        static std::mt19937_64 e2(rd());
        static const std::int64_t min_value = std::llround(std::pow(2,61));
        static const std::int64_t max_value = std::llround(std::pow(2,62));
        static std::uniform_int_distribution<std::int64_t> dist(min_value, max_value);
        return dist(e2);
    }

    id_t generate_random_id() {
        return generate_random_int64();
    }
}
