#include "utils/evlist_test.h"

#include <linux/input-event-codes.h>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

std::vector<std::pair<uint32_t, std::string>> evlist_test::createCapabilities(
) {
    return {
        std::make_pair(EV_SYN, std::to_string(EV_SYN)),
        std::make_pair(EV_KEY, std::to_string(EV_KEY)),
        std::make_pair(EV_REL, std::to_string(EV_REL)),
        std::make_pair(EV_MSC, std::to_string(EV_MSC))
    };
}
