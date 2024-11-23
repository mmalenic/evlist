
#include <format>
#include <iostream>
#include <utility>

#include "evlist/cli.h"
#include "evlist/list.h"

int main(int argc, char** argv) {
    auto cli = evlist::Cli{};
    auto exit = cli.parse(argc, argv);
    if (!exit.has_value()) {
        return exit.error();
    }
    if (*exit) {
        return 0;
    }

    auto devices =
        evlist::InputDeviceLister{cli.format(), std::move(cli).into_filter()}
            .list_input_devices();
    if (!devices.has_value()) {
        const auto& err = devices.error();
        std::cout << std::format("failed to list devices: {}", err.what());
        return err.code().value();
    }

    std::cout << std::format("{}", *devices);

    return 0;
}
