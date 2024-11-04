#include "evlist/list.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

#include "evlist/device.h"
#include "utils/evlist_test.h"

namespace fs = std::filesystem;

TEST(InputDeviceListerTest, ElevatedContainsAllDevices) {
    const evlist::InputDevices devices =
        evlist::InputDeviceLister{}.list_input_devices().value();

    std::vector<bool> results{};
    for (const auto &entry : fs::directory_iterator("/dev/input")) {
        if (entry.is_character_file() &&
            entry.path().filename().string().find("event") !=
                std::string::npos) {
            auto iter{devices.devices().begin()};
            for (const auto &device : devices.devices()) {
                if (device.device().string() == entry.path().string()) {
                    results.push_back(true);
                    break;
                }
            }
            if (iter == devices.devices().end()) {
                results.push_back(false);
            }
        }
    }
    ASSERT_TRUE(std::ranges::all_of(
        results.begin(), results.end(), [](const bool value) { return value; }
    ));
}

TEST(InputDeviceListerTest, ElevatedContainsAllIdSymlinks) {
    evlist_test::check_devices([](auto &device) { return device.by_id(); });
}

TEST(InputDeviceListerTest, ElevatedContainsAllPathSymlinks) {
    evlist_test::check_devices([](auto &device) { return device.by_path(); });
}
