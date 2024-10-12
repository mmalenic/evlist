
#include <iostream>
#include "evlist/InputDeviceLister.h"

int main(int argc, char* argv[]) {
    auto lister = ListInputDevices::InputDeviceLister{};

    std::cout << lister;
}
