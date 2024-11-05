#include "evlist/cli.h"

#include <iostream>
#include <map>
#include <string>

evlist::Cli::Cli()
    : app_{"lists and formats devices under /dev/input.", "evlist"} {}

int evlist::Cli::parse(int argc, char** argv) {
    argv = app_.ensure_utf8(argv);

    app_.add_option("-o,--format", format_, "Format to output devices in.")
        ->transform(CLI::CheckedTransformer{format_mappings(), CLI::ignore_case}
        );
    app_.add_option(
            "-f,--filter", filter_, "Filter output rows by the column value."
    )
        ->transform(CLI::CheckedTransformer{filter_mappings(), CLI::ignore_case}
        );

    CLI11_PARSE(app_, argc, argv);

    using CLI::enums::operator<<;
    std::cout << "format received: " << format_ << '\n';

    return 0;
}

evlist::Format evlist::Cli::format() const { return format_; }

const std::map<evlist::Filter, std::string>& evlist::Cli::filter() const {
    return filter_;
}

std::map<std::string, evlist::Format> evlist::Cli::format_mappings() {
    return {
        {"whitespace", Format::WHITESPACE},
        {"csv", Format::CSV},
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
