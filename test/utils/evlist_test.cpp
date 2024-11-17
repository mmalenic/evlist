#include "utils/evlist_test.h"

#include <linux/input-event-codes.h>

#include <string>
#include <vector>

std::vector<std::string> evlist_test::create_capabilities() {
    return {
        std::to_string(EV_SYN),
        std::to_string(EV_KEY),
        std::to_string(EV_REL),
        std::to_string(EV_MSC)
    };
}
