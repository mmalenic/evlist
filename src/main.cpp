
#include <format>
#include <iostream>

#include "evlist/list.h"

int main() {
    auto lister = evlist::InputDeviceLister{};

    std::cout << std::format("{}", *lister.listInputDevices());
}
