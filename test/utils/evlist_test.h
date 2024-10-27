#ifndef EVLIST_TEST_H
#define EVLIST_TEST_H

#include <gtest/gtest.h>

#include "evlist/device.h"
#include "evlist/list.h"

namespace evlist_test {

namespace fs = std::filesystem;

void checkDevices(auto &&getDevice) {
    evlist::InputDevices const devices =
        evlist::InputDeviceLister{}.listInputDevices().value();

    std::vector<bool> results{};
    for (auto &device : devices.devices()) {
        auto name = getDevice(device);
        if (name.has_value()) {
            const fs::path path{name.value()};
            results.push_back(
                fs::is_symlink(path) && fs::read_symlink(path).filename() ==
                                            device.getDevice().filename()
            );
        }
    }
    ASSERT_TRUE(all_of(results.begin(), results.end(), [](bool value) {
        return value;
    }));
}

std::vector<std::pair<int, std::string>> createCapabilities();
}  // namespace evlist_test

#endif  // EVLIST_TEST_H
