
#include <iostream>

#include "evlist/list.h"

int main(int argc, char *argv[]) {
    auto lister = ListInputDevices::InputDeviceLister{};

    std::cout << std::format("{}", lister.getInputDevices());
}
