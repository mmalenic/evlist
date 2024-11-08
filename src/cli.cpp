#include "evlist/cli.h"

#include <iostream>
#include <map>
#include <string>

evlist::Cli::Cli()
    : app_{"lists and formats devices under /dev/input.", "evlist"} {}

int evlist::Cli::parse(int argc, char** argv) {
    argv = app_.ensure_utf8(argv);

    app_.add_option("-o,--format", format_)
        ->transform(CLI::CheckedTransformer{format_mappings(), CLI::ignore_case}
        )
        ->option_text(format_enum(
            "FORMAT",
            "Format to output devices in",
            FORMAT_INDENT_BY,
            format_descriptions_
        ));
    app_.add_option("-f,--filter", filter_)
        ->transform(CLI::CheckedTransformer{filter_mappings(), CLI::ignore_case}
        )
        ->option_text(format_enum(
            "FILTER",
            "Filter output rows by the column value",
            FILTER_INDENT_BY,
            filter_descriptions_
        ));

    CLI11_PARSE(app_, argc, argv);

    return 0;
}

evlist::Format evlist::Cli::format() const { return format_; }

const std::map<evlist::Filter, std::string>& evlist::Cli::filter() const {
    return filter_;
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
