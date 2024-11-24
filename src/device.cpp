#include "evlist/device.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <filesystem>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "evlist/cli.h"

const evlist::fs::path& evlist::InputDevice::device_path() const {
    return device_;
}

evlist::InputDevice::InputDevice(
    fs::path device,
    std::optional<std::string> by_id,
    std::optional<std::string> by_path,
    std::optional<std::string> name,
    std::vector<std::string> capabilities
)
    : device_{std::move(device)},
      by_id_{std::move(by_id)},
      by_path_{std::move(by_path)},
      name_{std::move(name)},
      capabilities_{std::move(capabilities)} {}

const std::optional<std::string>& evlist::InputDevice::by_id() const {
    return by_id_;
}

const std::optional<std::string>& evlist::InputDevice::by_path() const {
    return by_path_;
}

const std::optional<std::string>& evlist::InputDevice::name() const {
    return name_;
}

const std::vector<std::string>& evlist::InputDevice::capabilities() const {
    return capabilities_;
}

std::vector<std::string> evlist::InputDevice::partition(std::string str) {
    if (str.empty()) {
        return {};
    }

    std::ranges::transform(str, str.begin(), [](auto character) {
        return std::tolower(character);
    });

    std::vector<std::string> result{};
    result.emplace_back(std::string{str[0]});
    for (auto i = 1; i < str.length(); i++) {
        auto prev = str[i - 1];
        auto curr = str[i];
        if (std::isdigit(prev) != 0 && std::isdigit(curr) != 0) {
            result[result.size() - 1] += curr;
        } else {
            result.emplace_back(std::string{curr});
        }
    }

    return result;
}

evlist::InputDevices::InputDevices(std::vector<InputDevice> devices)
    : devices_{std::move(devices)} {}

evlist::InputDevices::InputDevices(
    Format output_format, std::vector<InputDevice> input_devices
)
    : devices_{std::move(input_devices)}, output_format_{output_format} {}

evlist::InputDevices& evlist::InputDevices::filter(
    const std::map<Filter, std::string>& filter
) {
    auto filtered_devices = std::vector<InputDevice>{};
    for (const auto& [key, value] : filter) {
        for (auto device : devices_) {
            auto values = values_for_filter(device, key);
            if (values.contains(value)) {
                filtered_devices.emplace_back(std::move(device));
            }
        }
    }

    this->devices_ = filtered_devices;
    return *this;
}

std::set<std::string> evlist::InputDevices::values_for_filter(
    const InputDevice& device, Filter filter
) {
    switch (filter) {
        case Filter::DEVICE_PATH:
            return {device.device_path().c_str()};
        case Filter::NAME:
            return {device.name().value_or("")};
        case Filter::BY_ID:
            return {device.by_id().value_or("")};
        case Filter::BY_PATH:
            return {device.by_path().value_or("")};
        case Filter::CAPABILITIES:
            const auto& capabilities = device.capabilities();
            return {capabilities.cbegin(), capabilities.cend()};
    }

    return {};
}

evlist::InputDevices& evlist::InputDevices::with_max_name_size(
    std::size_t max_name_size
) {
    max_name_size_ += max_name_size;
    return *this;
}

evlist::InputDevices& evlist::InputDevices::with_max_device_size(
    std::size_t max_device_size
) {
    max_device_size_ += max_device_size;
    return *this;
}

evlist::InputDevices& evlist::InputDevices::with_max_by_id_size(
    std::size_t max_by_id_size
) {
    max_by_id_size_ += max_by_id_size;
    return *this;
}

evlist::InputDevices& evlist::InputDevices::with_max_by_path_size(
    std::size_t max_by_path_size
) {
    max_by_path_size_ += max_by_path_size;
    return *this;
}

std::vector<evlist::InputDevice> evlist::InputDevices::devices() const {
    return devices_;
}

std::size_t evlist::InputDevices::max_name_size() const {
    return max_name_size_;
}

std::size_t evlist::InputDevices::max_device_size() const {
    return max_device_size_;
}

std::size_t evlist::InputDevices::max_by_id_size() const {
    return max_by_id_size_;
}

std::size_t evlist::InputDevices::max_by_path_size() const {
    return max_by_path_size_;
}

evlist::Format evlist::InputDevices::output_format() const {
    return output_format_;
}
