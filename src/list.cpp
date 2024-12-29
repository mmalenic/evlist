#include "evlist/list.h"

#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <sys/ioctl.h>

#include <algorithm>
#include <array>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <expected>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

#include "evlist/cli.h"
#include "evlist/device.h"

#define STRINGIFY(x) #x

evlist::InputDeviceLister::InputDeviceLister(
    Format output_format,
    bool use_regex,
    std::vector<std::pair<Filter, std::string>> filter
)
    : output_format_{output_format},
      use_regex_{use_regex},
      filter_{std::move(filter)} {}

std::expected<evlist::InputDevices, std::filesystem::filesystem_error>
evlist::InputDeviceLister::list_input_devices() const {
    std::vector<InputDevice> devices{};

    for (const auto &entry : fs::directory_iterator(input_directory_)) {
        if (entry.is_character_file() &&
            entry.path().filename().string().contains("event")) {
            auto by_id_symlink = check_symlink(entry, by_id_);
            if (!by_id_symlink.has_value()) {
                return std::unexpected{by_id_symlink.error()};
            }
            auto by_path_symlink = check_symlink(entry, by_path_);
            if (!by_path_symlink.has_value()) {
                return std::unexpected{by_path_symlink.error()};
            }

            InputDevice device = {
                entry.path(),
                this->name(entry.path()),
                *by_id_symlink,
                *by_path_symlink,
                capabilities(entry.path())
            };
            devices.emplace_back(std::move(device));
        }
    }

    std::ranges::sort(devices, std::less{});

    auto inputDevices = InputDevices{output_format_, devices};

    if (!filter_.empty()) {
        inputDevices.filter(filter_, use_regex_);
    }

    return inputDevices;
}

std::
    expected<std::optional<evlist::fs::path>, std::filesystem::filesystem_error>
    evlist::InputDeviceLister::check_symlink(
        const fs::path &entry, const fs::path &path
    ) noexcept {
    try {
        for (const auto &path_enty : fs::directory_iterator(path)) {
            if (path_enty.is_symlink() &&
                read_symlink(path_enty.path()).filename() == entry.filename()) {
                return {{path_enty.path()}};
            }
        }
    } catch (fs::filesystem_error &err) {
        return std::unexpected{err};
    }
    return {};
}

std::vector<std::string> evlist::InputDeviceLister::capabilities(
    const fs::path &device
) const {
    std::array<std::uint64_t, EV_MAX> bit{};

    auto deleter = [](auto *file) {
        if (file != nullptr) {
            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
            return std::fclose(file);
        }
        return EOF;
    };
    auto file = std::unique_ptr<FILE, decltype(deleter)>{
        std::fopen(device.c_str(), "re"), deleter
    };

    if (file == nullptr) {
        return {};
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg,misc-include-cleaner)
    ioctl(fileno(file.get()), EVIOCGBIT(0, EV_MAX), bit.data());

    std::vector<std::string> out{};
    for (const auto &[first, second] : event_names_) {
        if ((bit.at(first / ULONG_WIDTH) & 1UL << first % ULONG_WIDTH) != 0U) {
            out.emplace_back(second);
        }
    }

    return out;
}

std::string evlist::InputDeviceLister::name(const fs::path &device) const {
    const fs::path fullPath = sys_class_ / device.filename() / name_path_;

    std::ifstream file{fullPath};
    std::string name{
        (std::istreambuf_iterator(file)), std::istreambuf_iterator<char>()
    };
    name.erase(
        std::ranges::begin(std::ranges::remove(name, '\n')), name.cend()
    );

    return name;
}

std::map<uint32_t, std::string> evlist::InputDeviceLister::event_names() {
    return {
        {EV_SYN, STRINGIFY(EV_SYN)},
        {EV_KEY, STRINGIFY(EV_KEY)},
        {EV_REL, STRINGIFY(EV_REL)},
        {EV_ABS, STRINGIFY(EV_ABS)},
        {EV_MSC, STRINGIFY(EV_MSC)},
        {EV_SW, STRINGIFY(EV_SW)},
        {EV_LED, STRINGIFY(EV_LED)},
        {EV_SND, STRINGIFY(EV_SND)},
        {EV_REP, STRINGIFY(EV_REP)},
        {EV_FF, STRINGIFY(EV_FF)},
        {EV_PWR, STRINGIFY(EV_PWR)},
        {EV_FF_STATUS, STRINGIFY(EV_FF_STATUS)},
        {EV_MAX, STRINGIFY(EV_MAX)},
    };
}
