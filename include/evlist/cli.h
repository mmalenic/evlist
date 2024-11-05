#ifndef EVLIST_CLI_H
#define EVLIST_CLI_H

#include <map>
#include <string>

#include "CLI/CLI.hpp"

namespace evlist {

/**
 * The format to output devices.
 */
enum class Format { WHITESPACE, CSV };

/**
 * The filter to apply to outputted devices.
 */
enum class Filter { DEVICE_PATH, NAME, BY_ID, BY_PATH, CAPABILITIES };

class Cli {
public:
    Cli();

    /**
     * Parse the CLI, exiting the program if there are any errors. This handles
     * printing messages from `--help` and exiting if there is a parse error.
     *
     * @param argc argc
     * @param argv argv
     * @return exit code
     */
    int parse(int argc, char** argv);

    /**
     * Get the format.
     * @return format
     */
    [[nodiscard]] Format format() const;

    /**
     * Get the filter.
     * @return filter
     */
    [[nodiscard]] const std::map<Filter, std::string>& filter() const;

private:
    CLI::App app_;

    Format format_{Format::WHITESPACE};
    std::map<Filter, std::string> filter_;

    static std::map<std::string, Format> format_mappings();
    static std::map<std::string, Filter> filter_mappings();
};
} // namespace evlist

#endif // EVLIST_CLI_H
