#ifndef EVLIST_CLI_H
#define EVLIST_CLI_H

#include <expected>
#include <map>
#include <string>

#include "CLI/CLI.hpp"

namespace evlist {

/**
 * The format to output devices.
 */
enum class Format : uint8_t { TABLE, CSV };

/**
 * The filter to apply to outputted devices.
 */
enum class Filter : uint8_t { DEVICE_PATH, NAME, BY_ID, BY_PATH, CAPABILITIES };

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
    std::expected<bool, int> parse(int argc, char** argv);

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
    static constexpr uint8_t INDENT_BY{30};
    static constexpr uint8_t FORMAT_INDENT_BY{8};
    static constexpr uint8_t FILTER_INDENT_BY{5};

    Format format_{Format::TABLE};
    std::map<Format, std::string> format_descriptions_{format_descriptions()};

    std::map<Filter, std::string> filter_;
    std::map<Filter, std::string> filter_descriptions_{filter_descriptions()};

    static std::map<std::string, Format> format_mappings();
    static std::map<Format, std::string> format_descriptions();

    static std::map<std::string, Filter> filter_mappings();
    static std::map<Filter, std::string> filter_descriptions();

    template <typename T>
    static std::string format_enum(
        std::string value_descriptor,
        std::string enum_description,
        uint8_t first_ident,
        const std::map<T, std::string>& descriptions
    );
};

template <typename T>
std::string Cli::format_enum(
    std::string value_descriptor,
    std::string enum_description,
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
