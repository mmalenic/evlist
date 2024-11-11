
#include <format>
#include <iostream>

#include "evlist/cli.h"
#include "evlist/list.h"

int main(int argc, char** argv) {
    auto cli = evlist::Cli{};
    auto exit = cli.parse(argc, argv);
    if (exit.should_exit) {
        return exit.exit_code;
    }

    auto lister = evlist::InputDeviceLister{cli.format()};

    std::cout << std::format("{}", *lister.list_input_devices());

    return 0;
}
