/**
 * @file cli.h
 *
 * Contains definitions for the command line interface.
 */

#ifndef EVLIST_CLI_H
#define EVLIST_CLI_H

#include <expected>
#include <format>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CLI/CLI.hpp"

/**
 * The namespace for this project.
 */
namespace evlist {

/**
 * The format to output devices in.
 */
enum class Format : uint8_t {
    /**
     * Output as a table separated by whitespace.
     */
    TABLE,
    /**
     * Output using CSV.
     */
    CSV
};

/**
 * The type of filter to filter devices by.
 */
enum class Filter : uint8_t {
    /**
     * Filter by the device path.
     */
    DEVICE_PATH,
    /**
     * Filter by the device name.
     */
    NAME,
    /**
     * Filter by the by-id path.
     */
    BY_ID,
    /**
     * Filter by the by-path path.
     */
    BY_PATH,
    /**
     * Filter by the device capabilities.
     */
    CAPABILITIES
};

/**
 * The command line options parser.
 */
class Cli {
public:
    /**
     * Create a new `Cli` parser.
     */
    Cli() = default;

    /**
     * Parse the CLI, exiting the program if there are any errors. This handles
     * printing messages from `--help` and exiting if there is a parse error.
     *
     * @param argc argc input argc
     * @param argv argv input argv
     * @return whether to exit or an error exit code
     */
    std::expected<bool, int> parse(int argc, char** argv);

    /**
     * Get the format.
     *
     * @return format
     */
    [[nodiscard]] Format format() const;

    /**
     * Get the use regex flag.
     *
     * @return use regex flag
     */
    [[nodiscard]] bool use_regex() const;

    /**
     * Get the filter.
     *
     * @return filter
     */
    [[nodiscard]] const std::vector<std::pair<Filter, std::string>>& filter(
    ) const;

    /**
     * Get the filter, consuming self.
     *
     * @return filter
     */
    [[nodiscard]] std::vector<std::pair<Filter, std::string>> into_filter() &&;

private:
    static constexpr uint8_t INDENT_BY{30};
    static constexpr uint8_t FORMAT_INDENT_BY{8};
    static constexpr uint8_t FILTER_INDENT_BY{5};

    Format format_{Format::TABLE};
    std::map<Format, std::string> format_descriptions_{format_descriptions()};

    std::vector<std::pair<Filter, std::string>> filter_;
    std::map<Filter, std::string> filter_descriptions_{filter_descriptions()};

    bool use_regex_{false};

    static std::map<std::string, Format> format_mappings();
    static std::map<Format, std::string> format_descriptions();

    static std::map<std::string, Filter> filter_mappings();
    static std::map<Filter, std::string> filter_descriptions();

    template <typename T>
    static std::string format_enum(
        const std::string& value_descriptor,
        const std::string& enum_description,
        uint8_t first_ident,
        const std::map<T, std::string>& descriptions
    );
};

template <typename T>
std::string Cli::format_enum(
    const std::string& value_descriptor,
    const std::string& enum_description,
    uint8_t first_ident,
    const std::map<T, std::string>& descriptions
) {
    auto out_description = std::format(
        "<{}>{: <{}}{}\n\n{: <{}}Possible values:\n",
        value_descriptor,
        "",
        first_ident,
        enum_description,
        "",
        INDENT_BY
    );
    for (const auto& [_, description] : descriptions) {
        out_description.append(
            std::format("{: <{}}{}\n", "", INDENT_BY, description)
        );
    }

    return out_description;
}

} // namespace evlist

#endif // EVLIST_CLI_H
