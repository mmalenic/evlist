#include "evlist/device.h"

#include <gtest/gtest.h>

#include <format>
#include <string>
#include <vector>

#include "evlist/cli.h"
#include "utils/evlist_test.h"

#define STRINGIFY(x) #x

TEST(InputDeviceTest, OrderWithAndWithoutSymlinks) {
    const evlist::InputDevice less{
        "/dev/input/event0", "", "", "", evlist_test::create_capabilities()
    };
    const evlist::InputDevice greater{
        "/dev/input/event3", {}, {}, "", evlist_test::create_capabilities()
    };
    ASSERT_LT(less, greater);
}

TEST(InputDeviceTest, OrderBothWithSymlinks) {
    const evlist::InputDevice less{
        "/dev/input/event3", "", "", "", evlist_test::create_capabilities()
    };
    const evlist::InputDevice greater{
        "/dev/input/event4", "", "", "", evlist_test::create_capabilities()
    };
    ASSERT_LT(less, greater);
}

TEST(InputDeviceTest, OrderBothWithoutSymlinks) {
    const evlist::InputDevice less{
        "/dev/input/event3", {}, {}, "", evlist_test::create_capabilities()
    };
    const evlist::InputDevice greater{
        "/dev/input/event4", {}, {}, "", evlist_test::create_capabilities()
    };
    ASSERT_LT(less, greater);
}

TEST(InputDeviceTest, OrderBothNaturalSort) {
    const evlist::InputDevice less{
        "/dev/input/event3", {}, {}, "", evlist_test::create_capabilities()
    };
    const evlist::InputDevice greater{
        "/dev/input/event10", {}, {}, "", evlist_test::create_capabilities()
    };

    ASSERT_LT(less, greater);
}

TEST(InputDeviceTest, Filter) {
    auto capabilities_first = evlist_test::create_capabilities();
    capabilities_first.emplace_back(STRINGIFY(EV_LED));
    const evlist::InputDevice first{
        "/dev/input/event3", {"by_id_3"}, {"by_path_3"}, "3", capabilities_first
    };
    const evlist::InputDevice second{
        "/dev/input/event10",
        {"by_id_10"},
        {"by_path_10"},
        "10",
        evlist_test::create_capabilities()
    };
    const evlist::InputDevice third{
        "/dev/input/event11",
        {"by_id_2_10"},
        {"by_path_2_10"},
        "11",
        evlist_test::create_capabilities()
    };

    auto path = evlist::InputDevices{std::vector{first, second, third}};
    path.filter({{evlist::Filter::DEVICE_PATH, "/dev/input/event3"}}, false);
    ASSERT_EQ(path.devices().size(), 1);
    ASSERT_EQ(path.devices()[0], first);

    auto path_regex = evlist::InputDevices{std::vector{first, second, third}};
    path_regex.filter(
        {{evlist::Filter::DEVICE_PATH, "/dev/input/event.$"}}, true
    );
    ASSERT_EQ(path_regex.devices().size(), 1);
    ASSERT_EQ(path_regex.devices()[0], first);

    auto by_id = evlist::InputDevices{std::vector{first, second, third}};
    by_id.filter({{evlist::Filter::BY_ID, "by_id_3"}}, false);
    ASSERT_EQ(by_id.devices().size(), 1);
    ASSERT_EQ(by_id.devices()[0], first);

    auto by_id_path = evlist::InputDevices{std::vector{first, second, third}};
    by_id_path.filter({{evlist::Filter::BY_ID, "by_id_.$"}}, true);
    ASSERT_EQ(by_id_path.devices().size(), 1);
    ASSERT_EQ(by_id_path.devices()[0], first);

    auto by_path = evlist::InputDevices{std::vector{first, second, third}};
    by_path.filter({{evlist::Filter::BY_PATH, "by_path_3"}}, false);
    ASSERT_EQ(by_path.devices().size(), 1);
    ASSERT_EQ(by_path.devices()[0], first);

    auto by_path_regex =
        evlist::InputDevices{std::vector{first, second, third}};
    by_path_regex.filter({{evlist::Filter::BY_PATH, "by_path_.$"}}, true);
    ASSERT_EQ(by_path_regex.devices().size(), 1);
    ASSERT_EQ(by_path_regex.devices()[0], first);

    auto name = evlist::InputDevices{std::vector{first, second, third}};
    name.filter({{evlist::Filter::NAME, "3"}}, false);
    ASSERT_EQ(name.devices().size(), 1);
    ASSERT_EQ(name.devices()[0], first);

    auto name_regex = evlist::InputDevices{std::vector{first, second, third}};
    name_regex.filter({{evlist::Filter::NAME, "^.$"}}, true);
    ASSERT_EQ(name_regex.devices().size(), 1);
    ASSERT_EQ(name_regex.devices()[0], first);

    auto combined = evlist::InputDevices{std::vector{first, second, third}};
    combined.filter(
        {{evlist::Filter::DEVICE_PATH, "/dev/input/event3"},
         {evlist::Filter::NAME, "3"}},
        false
    );
    ASSERT_EQ(combined.devices().size(), 1);
    ASSERT_EQ(combined.devices()[0], first);

    auto combined_regex =
        evlist::InputDevices{std::vector{first, second, third}};
    combined_regex.filter(
        {{evlist::Filter::DEVICE_PATH, "/dev/input/event.$"},
         {evlist::Filter::NAME, "^.$"}},
        true
    );
    ASSERT_EQ(combined_regex.devices().size(), 1);
    ASSERT_EQ(combined_regex.devices()[0], first);

    auto capabilities = evlist::InputDevices{std::vector{first, second, third}};
    capabilities.filter(
        {{evlist::Filter::CAPABILITIES, "EV_LED"},
         {evlist::Filter::CAPABILITIES, "EV_SYN"}},
        false
    );
    ASSERT_EQ(capabilities.devices().size(), 1);
    ASSERT_EQ(capabilities.devices()[0], first);

    auto capabilities_regex =
        evlist::InputDevices{std::vector{first, second, third}};
    capabilities_regex.filter(
        {{evlist::Filter::CAPABILITIES, "EV_LE.$"},
         {evlist::Filter::CAPABILITIES, "EV_SY.$"}},
        true
    );
    ASSERT_EQ(capabilities_regex.devices().size(), 1);
    ASSERT_EQ(capabilities_regex.devices()[0], first);

    auto combined_same_filter =
        evlist::InputDevices{std::vector{first, second, third}};
    combined_same_filter.filter(
        {{evlist::Filter::BY_ID, "^by_id_.*"},
         {evlist::Filter::BY_ID, ".*d_10$"}},
        true
    );
    ASSERT_EQ(combined_same_filter.devices().size(), 1);
    ASSERT_EQ(combined_same_filter.devices()[0], second);

    auto combined_not_found =
        evlist::InputDevices{std::vector{first, second, third}};
    combined_not_found.filter(
        {{evlist::Filter::DEVICE_PATH, "/dev/input/event3"},
         {evlist::Filter::BY_PATH, "by_path_10"}},
        false
    );
    ASSERT_EQ(combined_not_found.devices().size(), 0);

    auto combined_not_found_regex =
        evlist::InputDevices{std::vector{first, second, third}};
    combined_not_found_regex.filter(
        {{evlist::Filter::DEVICE_PATH, "/dev/input/event.$"},
         {evlist::Filter::BY_PATH, "by_path_..$"}},
        true
    );
    ASSERT_EQ(combined_not_found_regex.devices().size(), 0);
}

TEST(InputDeviceTest, Format) {
    std::string input{"event"};
    auto capabilities = evlist_test::create_capabilities();

    const evlist::InputDevices devices_table{
        evlist::Format::TABLE, {{input, input, input, input, capabilities}}
    };
    ASSERT_EQ(
        std::format("{}", devices_table),
        "NAME  DEVICE_PATH BY_ID BY_PATH CAPABILITIES\n"
        "event event       event event   [EV_SYN, EV_KEY, EV_REL, EV_MSC]\n"
    );

    const evlist::InputDevices devices_csv{
        evlist::Format::CSV, {{input, input, input, input, capabilities}}
    };
    ASSERT_EQ(
        std::format("{}", devices_csv),
        R"("NAME","DEVICE_PATH","BY_ID","BY_PATH","CAPABILITIES")"
        "\n"
        R"("event","event","event","event","[EV_SYN, EV_KEY, EV_REL, EV_MSC]")"
        "\n"
    );
}
