
#include <format>
#include <iostream>

#include "evlist/cli.h"
#include "evlist/list.h"

int main(int argc, char** argv) {
    auto cli = evlist::Cli{};
    cli.parse(argc, argv);

    auto lister = evlist::InputDeviceLister{};

    std::cout << std::format("{}", *lister.list_input_devices());
}
