#ifndef EVLIST_UTILS_TEST_H
#define EVLIST_UTILS_TEST_H

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "evlist/device.h"
#include "evlist/list.h"

namespace evlist_test {

namespace fs = std::filesystem;

void check_devices(auto &&get_device) {
    evlist::InputDevices const devices =
        evlist::InputDeviceLister{}.list_input_devices().value();

    std::vector<bool> results{};
    for (auto &device : devices.devices()) {
        auto name = get_device(device);
        if (name.has_value()) {
            const fs::path path{name.value()};
            results.push_back(
                fs::is_symlink(path) &&
                fs::read_symlink(path).filename() == device.device().filename()
            );
        }
    }
    ASSERT_TRUE(all_of(results.begin(), results.end(), [](bool value) {
        return value;
    }));
}

std::vector<std::pair<uint32_t, std::string>> create_capabilities();
} // namespace evlist_test

#endif // EVLIST_UTILS_TEST_H
