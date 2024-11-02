#include "evlist/device.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <utility>
#include <vector>

const evlist::fs::path& evlist::InputDevice::getDevice() const {
    return device;
}

evlist::InputDevice::InputDevice(
    fs::path device,
    std::optional<std::string> byId,
    std::optional<std::string> byPath,
    std::optional<std::string> name,
    std::vector<std::pair<int, std::string>> capabilities
)
    : device{std::move(device)},
      byId{std::move(byId)},
      byPath{std::move(byPath)},
      name{std::move(name)},
      capabilities{std::move(capabilities)} {}

const std::optional<std::string>& evlist::InputDevice::getById() const {
    return byId;
}

const std::optional<std::string>& evlist::InputDevice::getByPath() const {
    return byPath;
}

const std::optional<std::string>& evlist::InputDevice::getName() const {
    return name;
}

const std::vector<std::pair<int, std::string>>&
evlist::InputDevice::getCapabilities() const {
    return capabilities;
}

std::vector<std::string> evlist::InputDevice::partition(std::string str) {
    if (str.empty()) {
        return {};
    }

    std::ranges::transform(str, str.begin(), [](auto character) {
        return std::tolower(character);
    });

    std::vector<std::string> result{};
    result.push_back({str[0]});
    for (auto i = 1; i < str.length(); i++) {
        auto prev = str[i - 1];
        auto curr = str[i];
        if (std::isdigit(prev) != 0 && std::isdigit(curr) != 0) {
            result[result.size() - 1] += curr;
        } else {
            result.push_back({curr});
        }
    }

    return result;
}

evlist::InputDevices::InputDevices(std::vector<InputDevice> devices)
    : _devices{std::move(devices)} {}

evlist::InputDevices& evlist::InputDevices::with_max_name_size(
    std::size_t max_name_size
) {
    _max_name_size += max_name_size;
    return *this;
}

evlist::InputDevices& evlist::InputDevices::with_max_device_size(
    std::size_t max_device_size
) {
    _max_device_size += max_device_size;
    return *this;
}

evlist::InputDevices& evlist::InputDevices::with_max_by_id_size(
    std::size_t max_by_id_size
) {
    _max_by_id_size += max_by_id_size;
    return *this;
}

evlist::InputDevices& evlist::InputDevices::with_max_by_path_size(
    std::size_t max_by_path_size
) {
    _max_by_path_size += max_by_path_size;
    return *this;
}

std::vector<evlist::InputDevice> evlist::InputDevices::devices() const {
    return _devices;
}

std::size_t evlist::InputDevices::max_name_size() const {
    return _max_name_size;
}

std::size_t evlist::InputDevices::max_device_size() const {
    return _max_device_size;
}

std::size_t evlist::InputDevices::max_by_id_size() const {
    return _max_by_id_size;
}

std::size_t evlist::InputDevices::max_by_path_size() const {
    return _max_by_path_size;
}
