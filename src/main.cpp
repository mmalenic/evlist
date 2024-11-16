
#include <format>
#include <iostream>

#include "evlist/cli.h"
#include "evlist/list.h"

int main(int argc, char** argv) {
    auto exit = evlist::Cli{}.parse(argc, argv);
    if (!exit.has_value()) {
        return exit.error();
    }
    if (*exit) {
        return 0;
    }

    auto devices =
        evlist::InputDeviceLister{evlist::Format::CSV}.list_input_devices();
    if (!devices.has_value()) {
        const auto& err = devices.error();
        std::cout << std::format("failed to list devices: {}", err.what());
        return err.code().value();
    }

    std::cout << std::format("{}", *devices);

    return 0;
}
