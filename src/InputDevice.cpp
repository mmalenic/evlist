// MIT License
//
// Copyright (c) 2021 Marko Malenic
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "evlist/InputDevice.h"

#include <ranges>
#include <regex>
#include <utility>

static constexpr size_t MIN_SPACE_GAP = 4;
static constexpr size_t SPACE_FOR_SYMLINK = 2;
static constexpr char BY_ID[] = "by-id";
static constexpr char BY_PATH[] = "by-path";

size_t ListInputDevices::InputDevice::maxNameSize = 0;
size_t ListInputDevices::InputDevice::maxPathSize = 0;

const ListInputDevices::fs::path& ListInputDevices::InputDevice::getDevice() const {
    return device;
}

ListInputDevices::InputDevice::InputDevice(
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

const std::optional<std::string>& ListInputDevices::InputDevice::getById() const {
    return byId;
}

const std::optional<std::string>& ListInputDevices::InputDevice::getByPath() const {
    return byPath;
}

const std::optional<std::string>& ListInputDevices::InputDevice::getName() const {
    return name;
}

const std::vector<std::pair<int, std::string>>& ListInputDevices::InputDevice::getCapabilities() const {
    return capabilities;
}

size_t ListInputDevices::InputDevice::getMaxNameSize() {
    return maxNameSize;
}

void ListInputDevices::InputDevice::setMaxNameSize(size_t newMaxNameSize) {
    maxNameSize = newMaxNameSize;
}

size_t ListInputDevices::InputDevice::getMaxPathSize() {
    return maxPathSize;
}

void ListInputDevices::InputDevice::setMaxPathSize(size_t newMaxPathSize) {
    maxPathSize = newMaxPathSize;
}

std::vector<std::string>
ListInputDevices::InputDevice::partition(std::string s) {
    if (s.empty()) {
        return {};
    }

    std::transform(s.begin(), s.end(), s.begin(), [](const unsigned char c){ return std::tolower(c); });

    std::vector<std::string> result{};
    result.push_back({s[0]});
    for (auto i = 1; i < s.length(); i++) {
        auto prev = s[i - 1];
        auto curr = s[i];
        if (std::isdigit(prev) && std::isdigit(curr)) {
            result[result.size() - 1] += curr;
        } else {
            result.push_back({curr});
        }
    }

    return result;
}


std::partial_ordering ListInputDevices::InputDevice::operator<=>(const InputDevice& eventDevice) const {
    auto s1_partitions = partition(device.string());
    auto s2_partitions = partition(eventDevice.device.string());

    for (auto [a, b] : std::views::zip(s1_partitions, s2_partitions)) {
        if (a != b && !a.empty() && !b.empty()) {
            if (isdigit(a[0]) && isdigit(b[0])) {
                return std::stoi(a) <=> std::stoi(b);
            }
            return a <=> b;
        }
    }

    return device.string() <=> eventDevice.device.string();
}

std::ostream& ListInputDevices::operator<<(std::ostream& os, const InputDevice& eventDevice) {
    std::string name = "";
    if (eventDevice.name.has_value()) {
        name = *eventDevice.name;
    }

    std::string path = eventDevice.device.string();

    os << std::format(
        "{: <{}}{: <{}}\n",
        name,
        MIN_SPACE_GAP + InputDevice::maxNameSize,
        path,
        MIN_SPACE_GAP + InputDevice::maxPathSize
        // *eventDevice.getById(),
        // 10,
        // *eventDevice.getByPath(),
        // 10
        );
    // size_t deviceNameLength = 0;
    // size_t devicePathLength = eventDevice.device.string().length();
    // if (eventDevice.name.has_value()) {
    //     os << eventDevice.name.value();
    //     deviceNameLength = eventDevice.name->length();
    // }
    // auto totalName = MIN_SPACE_GAP + InputDevice::maxNameSize;
    // auto spacesName = totalName - deviceNameLength;
    // auto spacesPath = (MIN_SPACE_GAP + InputDevice::maxPathSize) - devicePathLength;
    // os << std::string(1, '\t') << eventDevice.device.string() << std::string(1, '\t');
    //
    // if (!eventDevice.capabilities.empty()) {
    //     os << "capabilities = [";
    //     for (auto i{eventDevice.capabilities.begin()}; i != --eventDevice.capabilities.end(); ++i) {
    //         os << "(" << i->first << "; " << i->second << "), ";
    //     }
    //     os << "(" << eventDevice.capabilities.back().first << "; " << eventDevice.capabilities.back().second << ")]";
    // }
    // os << "\n";
    //
    // if (eventDevice.byId.has_value()) {
    //     auto spacesById = totalName - std::size(BY_ID) + SPACE_FOR_SYMLINK;
    //     os << "  " << BY_ID << std::string(spacesById, ' ') << "<- " << eventDevice.byId.value() << "\n";
    // }
    // if (eventDevice.byPath.has_value()) {
    //     auto spacesByPath = totalName - std::size(BY_PATH) + SPACE_FOR_SYMLINK;
    //     os << "  " << BY_PATH << std::string(spacesByPath, ' ') << "<- " << eventDevice.byPath.value() << "\n";
    // }
    // return os;

    return os;
}

ListInputDevices::InputDevices::InputDevices(std::vector<InputDevice> devices)
    : _devices{std::move(devices)} {}

void ListInputDevices::InputDevices::add_max_name_size(size_t max_name_size) {
    _max_name_size += max_name_size;
}

void ListInputDevices::InputDevices::add_max_device_size(size_t max_device_size
) {
    _max_device_size += max_device_size;
}

void ListInputDevices::InputDevices::add_max_by_id_size(size_t max_by_id_size) {
    _max_by_id_size += max_by_id_size;
}

void ListInputDevices::InputDevices::add_max_by_path_size(
    size_t max_by_path_size
) {
    _max_by_path_size += max_by_path_size;
}

std::vector<ListInputDevices::InputDevice>
ListInputDevices::InputDevices::devices() const {
    return _devices;
}

size_t ListInputDevices::InputDevices::max_name_size() const {
    return _max_name_size;
}

size_t ListInputDevices::InputDevices::max_device_size() const {
    return _max_device_size;
}

size_t ListInputDevices::InputDevices::max_by_id_size() const {
    return _max_by_id_size;
}

size_t ListInputDevices::InputDevices::max_by_path_size() const {
    return _max_by_path_size;
}