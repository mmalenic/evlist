#include "evlist/device.h"

#include <gtest/gtest.h>

#include "utils/evlist_test.h"

TEST(InputDeviceTest, OrderWithAndWithoutSymlinks) {
    const evlist::InputDevice less{
        "/dev/input/event0", "", "", "", evlist_test::createCapabilities()
    };
    const evlist::InputDevice greater{
        "/dev/input/event3", {}, {}, "", evlist_test::createCapabilities()
    };
    ASSERT_LT(less, greater);
}

TEST(InputDeviceTest, OrderBothWithSymlinks) {
    const evlist::InputDevice less{
        "/dev/input/event3", "", "", "", evlist_test::createCapabilities()
    };
    const evlist::InputDevice greater{
        "/dev/input/event4", "", "", "", evlist_test::createCapabilities()
    };
    ASSERT_LT(less, greater);
}

TEST(InputDeviceTest, OrderBothWithoutSymlinks) {
    const evlist::InputDevice less{
        "/dev/input/event3", {}, {}, "", evlist_test::createCapabilities()
    };
    const evlist::InputDevice greater{
        "/dev/input/event4", {}, {}, "", evlist_test::createCapabilities()
    };
    ASSERT_LT(less, greater);
}

TEST(InputDeviceTest, OrderBothNaturalSort) {
    const evlist::InputDevice less{
        "/dev/input/event3", {}, {}, "", evlist_test::createCapabilities()
    };
    const evlist::InputDevice greater{
        "/dev/input/event10", {}, {}, "", evlist_test::createCapabilities()
    };

    ASSERT_LT(less, greater);
}
