#include "evlist/list.h"

#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <sys/ioctl.h>

#include <algorithm>
#include <array>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <expected>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "evlist/device.h"

#define ULONG_BITS (CHAR_BIT * sizeof(unsigned long))
#define STRINGIFY(x) #x

evlist::InputDeviceLister::InputDeviceLister()
    : inputDirectory{"/dev/input"},
      byId{inputDirectory / "by-id"},
      byPath{inputDirectory / "by-path"},
      sysClass{"/sys/class/input"},
      namePath{"device/name"},
      eventCodeToName{getEventCodeToName()},
      inputDevices{*listInputDevices()} {}

std::expected<evlist::InputDevices, std::filesystem::filesystem_error>
evlist::InputDeviceLister::listInputDevices() {
    std::size_t max_name_size = 0;
    std::size_t max_device_size = 0;
    std::size_t max_by_id_size = 0;
    std::size_t max_by_path_size = 0;

    std::vector<InputDevice> devices{};
    for (const auto &entry : fs::directory_iterator(inputDirectory)) {
        if (entry.is_character_file() &&
            entry.path().filename().string().contains("event")) {
            auto name = getName(entry.path());

            auto byIdSymlink = checkSymlink(entry, byId);
            if (!byIdSymlink.has_value()) {
                return std::unexpected{byIdSymlink.error()};
            }
            auto byPathSymlink = checkSymlink(entry, byPath);
            if (!byPathSymlink.has_value()) {
                return std::unexpected{byPathSymlink.error()};
            }

            InputDevice const device = {
                entry.path(),
                *byIdSymlink,
                *byPathSymlink,
                getName(entry.path()),
                getCapabilities(entry.path())
            };

            max_name_size = std::ranges::max(max_name_size, name.length());
            max_device_size = std::ranges::max(
                max_device_size, entry.path().string().length()
            );
            max_by_id_size = std::ranges::max(
                max_by_id_size,
                byIdSymlink->value_or(fs::path{}).string().length()
            );
            max_by_path_size = std::ranges::max(
                max_by_path_size,
                byPathSymlink->value_or(fs::path{}).string().length()
            );

            devices.push_back(device);
        }
    }

    std::ranges::sort(devices, std::less{});

    auto inputDevices = InputDevices{devices};
    inputDevices.add_max_name_size(max_name_size);
    inputDevices.add_max_device_size(max_device_size);
    inputDevices.add_max_by_id_size(max_by_id_size);
    inputDevices.add_max_by_path_size(max_by_path_size);

    return inputDevices;
}

const evlist::InputDevices &evlist::InputDeviceLister::getInputDevices() const {
    return inputDevices;
}

std::
    expected<std::optional<evlist::fs::path>, std::filesystem::filesystem_error>
    evlist::InputDeviceLister::checkSymlink(
        const fs::path &entry, const fs::path &path
    ) noexcept {
    try {
        for (const auto &symEntry : fs::directory_iterator(path)) {
            if (symEntry.is_symlink() &&
                read_symlink(symEntry.path()).filename() == entry.filename()) {
                return {{symEntry.path()}};
            }
        }
    } catch (fs::filesystem_error &err) {
        return std::unexpected{err};
    }
    return {};
}

std::vector<std::pair<int, std::string>>
evlist::InputDeviceLister::getCapabilities(const fs::path &device) const {
    std::array<std::uint64_t, EV_MAX> bit{};

    auto file = std::unique_ptr<FILE, decltype(&fclose)>{
        std::fopen(device.c_str(), "re"),
        [](auto *file) {
            if (file != nullptr) {
                // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
                return std::fclose(file);
            }
            return EOF;
        }
    };

    if (file == nullptr) {
        return {};
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg,misc-include-cleaner)
    ioctl(fileno(file.get()), EVIOCGBIT(0, EV_MAX), bit.data());

    std::vector<std::pair<int, std::string>> vec{};
    for (const auto &type : eventCodeToName) {
        if ((bit.at(type.first / ULONG_BITS) & 1UL << type.first % ULONG_BITS
            ) != 0U) {
            vec.emplace_back(type);
        }
    }

    return vec;
}

std::string evlist::InputDeviceLister::getName(const fs::path &device) {
    const fs::path fullPath = sysClass / device.filename() / namePath;
    std::string name = fullPath.string();

    maxNameSize = std::max(name.length(), maxNameSize);

    return name;
}

std::map<int, std::string> evlist::InputDeviceLister::getEventCodeToName() {
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
