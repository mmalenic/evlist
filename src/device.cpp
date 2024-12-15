#include "evlist/device.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <filesystem>
#include <map>
#include <optional>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "evlist/cli.h"

const evlist::fs::path& evlist::InputDevice::device_path() const {
    return device_;
}

evlist::InputDevice::InputDevice(
    fs::path device,
    std::string name,
    std::optional<std::string> by_id,
    std::optional<std::string> by_path,
    std::vector<std::string> capabilities
)
    : device_{std::move(device)},
      name_{std::move(name)},
      by_id_{std::move(by_id)},
      by_path_{std::move(by_path)},
      capabilities_{std::move(capabilities)} {}

const std::optional<std::string>& evlist::InputDevice::by_id() const {
    return by_id_;
}

const std::optional<std::string>& evlist::InputDevice::by_path() const {
    return by_path_;
}

const std::string& evlist::InputDevice::name() const { return name_; }

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
    : output_format_{output_format} {
    for (const auto& device : input_devices) {
        with_max_name(device.name().length());
        with_max_device_path(device.device_path().string().length());
        with_max_by_id(device.by_id().value_or("").length());
        with_max_by_path(device.by_path().value_or("").length());
    }

    with_input_devices(std::move(input_devices));
}

evlist::InputDevices& evlist::InputDevices::filter(
    const std::vector<std::pair<Filter, std::string>>& filter, bool use_regex
) {
    auto capacity = devices_.size();
    auto devices = std::move(devices_);
    this->devices_ = {};
    this->devices_.reserve(capacity);

    for (auto device : devices) {
        auto keep_device = std::ranges::all_of(
            filter,
            [&use_regex, &device, this](const auto& filter) {
                if (use_regex) {
                    if (filter_regex(device, filter.first, filter.second)) {
                        return true;
                    }
                } else {
                    if (filter_equality(device, filter.first, filter.second)) {
                        return true;
                    }
                }

                return false;
            }
        );

        if (keep_device) {
            this->devices_.emplace_back(std::move(device));
        }
    }

    return *this;
}

bool evlist::InputDevices::filter_regex(
    const InputDevice& device, Filter filter, const std::string& value
) {
    if (!regexes.contains(value)) {
        regexes.emplace(value, std::regex{value});
    }
    auto& regex = regexes[value];

    return filter_device(device, filter, [&regex](const auto& compare) {
        return std::regex_search(compare, regex);
    });
}

bool evlist::InputDevices::filter_equality(
    const InputDevice& device, Filter filter, const std::string& value
) {
    return filter_device(device, filter, [&value](const auto& compare) {
        return compare == value;
    });
}

evlist::InputDevices& evlist::InputDevices::with_max_name(
    std::size_t max_name_size
) {
    max_name_size_ =
        std::ranges::max(max_name_size_, max_name_size + MIN_SPACES);
    return *this;
}

evlist::InputDevices& evlist::InputDevices::with_max_device_path(
    std::size_t max_device_size
) {
    max_device_size_ =
        std::ranges::max(max_device_size_, max_device_size + MIN_SPACES);
    return *this;
}

evlist::InputDevices& evlist::InputDevices::with_max_by_id(
    std::size_t max_by_id_size
) {
    max_by_id_size_ =
        std::ranges::max(max_by_id_size_, max_by_id_size + MIN_SPACES);
    return *this;
}

evlist::InputDevices& evlist::InputDevices::with_max_by_path(
    std::size_t max_by_path_size
) {
    max_by_path_size_ =
        std::ranges::max(max_by_path_size_, max_by_path_size + MIN_SPACES);
    return *this;
}

evlist::InputDevices& evlist::InputDevices::with_input_devices(
    std::vector<InputDevice> input_devices
) {
    devices_ = std::move(input_devices);
    return *this;
}

std::vector<evlist::InputDevice> evlist::InputDevices::devices() const {
    return devices_;
}

std::size_t evlist::InputDevices::max_name() const { return max_name_size_; }

std::size_t evlist::InputDevices::max_device_path() const {
    return max_device_size_;
}

std::size_t evlist::InputDevices::max_by_id() const { return max_by_id_size_; }

std::size_t evlist::InputDevices::max_by_path() const {
    return max_by_path_size_;
}

evlist::Format evlist::InputDevices::output_format() const {
    return output_format_;
}
