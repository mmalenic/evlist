#ifndef EVLIST_CLI_H
#define EVLIST_CLI_H

#include "CLI/CLI.hpp"

namespace evlist {
class Cli {
public:
    Cli() = default;

    /**
     * Parse the CLI, exiting the program if there are any errors. This handles
     * printing messages from `--help` and exiting if there is a parse error.
     *
     * @param argc argc
     * @param argv argv
     * @return exit code
     */
    int parse(int argc, char** argv);

private:
    CLI::App app_;
};
}  // namespace evlist

#endif  // EVLIST_CLI_H
