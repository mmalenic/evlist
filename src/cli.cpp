#include "evlist/cli.h"

#include <expected>
#include <format>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "CLI/CLI.hpp"

std::expected<bool, int> evlist::Cli::parse(int argc, char** argv) {
    CLI::App app{"lists and formats devices under /dev/input.", "evlist"};
    argv = app.ensure_utf8(argv);

    auto should_exit = false;
    app.add_flag_callback(
        "-v,--version",
        [&should_exit] {
            should_exit = true;
            std::cout << std::format(
                "evlist {}\n\n{}\n{}\n",
                EVLIST_VERSION,
                EVLIST_LICENSE,
                EVLIST_COPYRIGHT
            );
        },
        "Print version"
    );

    app.add_option("-o,--format", format_)
        ->transform(CLI::Transformer{format_mappings(), CLI::ignore_case})
        ->option_text(format_enum(
            "FORMAT",
            "Format to output devices in",
            FORMAT_INDENT_BY,
            format_descriptions_
        ));

    app.add_option("-f,--filter", filter_)
        ->transform(CLI::Transformer{filter_mappings(), CLI::ignore_case})
        ->delimiter('=')
        ->option_text(format_enum(
            "KEY=VALUE",
            "Filter output rows by the column value. This "
            "option can be specified multiple times, and takes a key=value "
            "format where each key is one of the following",
            FILTER_INDENT_BY,
            filter_descriptions_
        ));

    app.add_flag(
        "-r,--use-regex",
        use_regex_,
        "Pass this to use a regex when filtering values in `--filter` options"
    );

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return std::unexpected{app.exit(e)};
    }

    return {should_exit};
}

evlist::Format evlist::Cli::format() const { return format_; }

bool evlist::Cli::use_regex() const { return use_regex_; }

const std::vector<std::pair<evlist::Filter, std::string>>& evlist::Cli::filter(
) const {
    return filter_;
}

std::vector<std::pair<evlist::Filter, std::string>> evlist::Cli::into_filter(
) && {
    return std::move(filter_);
}

std::map<std::string, evlist::Format> evlist::Cli::format_mappings() {
    return {
        {"table", Format::TABLE},
        {"csv", Format::CSV},
    };
}

std::map<evlist::Format, std::string> evlist::Cli::format_descriptions() {
    return {
        {Format::TABLE, "- table: format the output as a table"},
        {Format::CSV, "- csv: format the output as CSV"},
    };
}
std::map<evlist::Filter, std::string> evlist::Cli::filter_descriptions() {
    return {
        {Filter::DEVICE_PATH,
         "- device_path: filter outputs that contain the device path"},
        {Filter::NAME,
         "- name: filter outputs that contain the name of the device"},
        {Filter::BY_ID,
         "- by_id: filter outputs that contain the by_id path of the device"},
        {Filter::BY_PATH,
         "- by_path: filter outputs that contain the by_path path of the device"
        },
        {Filter::CAPABILITIES,
         "- capabilities: filter outputs that have the capabilities listed"},
    };
}

std::map<std::string, evlist::Filter> evlist::Cli::filter_mappings() {
    return {
        {"device_path", Filter::DEVICE_PATH},
        {"name", Filter::NAME},
        {"by_id", Filter::BY_ID},
        {"by_path", Filter::BY_PATH},
        {"capabilities", Filter::CAPABILITIES},
    };
}
