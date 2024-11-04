#include "evlist/cli.h"

int evlist::Cli::parse(int argc, char** argv) {
    CLI11_PARSE(app_, argc, argv);
    return 0;
}
