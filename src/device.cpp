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
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "evlist/device.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <utility>
#include <vector>

const evlist::fs::path &evlist::InputDevice::getDevice() const {
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

const std::optional<std::string> &evlist::InputDevice::getById() const {
    return byId;
}

const std::optional<std::string> &evlist::InputDevice::getByPath() const {
    return byPath;
}

const std::optional<std::string> &evlist::InputDevice::getName() const {
    return name;
}

const std::vector<std::pair<int, std::string>> &
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

void evlist::InputDevices::add_max_name_size(std::size_t max_name_size) {
    _max_name_size += max_name_size;
}

void evlist::InputDevices::add_max_device_size(std::size_t max_device_size) {
    _max_device_size += max_device_size;
}

void evlist::InputDevices::add_max_by_id_size(std::size_t max_by_id_size) {
    _max_by_id_size += max_by_id_size;
}

void evlist::InputDevices::add_max_by_path_size(std::size_t max_by_path_size) {
    _max_by_path_size += max_by_path_size;
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
