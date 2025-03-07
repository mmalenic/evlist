#include "common/common.h"

#include <string>
#include <vector>

#define STRINGIFY(x) #x

std::vector<std::string> evlist::create_capabilities() {
    return {
        STRINGIFY(EV_SYN),
        STRINGIFY(EV_KEY),
        STRINGIFY(EV_REL),
        STRINGIFY(EV_MSC)
    };
}
