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

#ifndef INPUT_EVENT_RECORDER_EVENTDEVICE_H
#define INPUT_EVENT_RECORDER_EVENTDEVICE_H

#include <filesystem>
#include <optional>
#include <vector>
#include <format>

namespace ListInputDevices {
namespace fs = std::filesystem;

/**
 * Store the paths for event devices.
 */
class InputDevice {
public:
    /**
     * Create event device.
     * @param device device path
     * @param byId by id path
     * @param byPath by path path
     * @param name device name
     * @param capabilities capabilities
     */
    InputDevice(
        fs::path device,
        std::optional<std::string> byId,
        std::optional<std::string> byPath,
        std::optional<std::string> name,
        std::vector<std::pair<int, std::string>> capabilities
    );

    /**
     * Get device.
     * @return device
     */
    [[nodiscard]] const fs::path& getDevice() const;

    /**
     * Get by id.
     * @return by id
     */
    [[nodiscard]] const std::optional<std::string>& getById() const;

    /**
     * Get by path.
     * @return by path
     */
    [[nodiscard]] const std::optional<std::string>& getByPath() const;

    /**
     * Get name.
     * @return name
     */
    [[nodiscard]] const std::optional<std::string>& getName() const;

    /**
     * Get capabilities.
     * @return capabilities
     */
    [[nodiscard]] const std::vector<std::pair<int, std::string>>& getCapabilities() const;

    /**
     * Get max name size.
     * @return max name size
     */
    static size_t getMaxNameSize();

    /**
     * Set max name size.
     * @param newMaxNameSize max name size
     */
    static void setMaxNameSize(size_t newMaxNameSize);

    /**
     * Get max path size.
     * @return max path size
     */
    static size_t getMaxPathSize();

    /**
     * Set max path size.
     * @param newMaxPathSize max path size
     */
    static void setMaxPathSize(size_t newMaxPathSize);

    std::partial_ordering operator<=>(const InputDevice& eventDevice) const;
    friend std::ostream& operator<<(std::ostream& os, const InputDevice& deviceLister);

private:
    fs::path device;
    std::optional<std::string> byId;
    std::optional<std::string> byPath;
    std::optional<std::string> name;
    std::vector<std::pair<int, std::string>> capabilities;
    static size_t maxNameSize;
    static size_t maxPathSize;

    /**
     * Partition a string into continuous segments of numbers of characters.
     */
    [[nodiscard]] static std::vector<std::string> partition(std::string s) ;
};

/**
 * A list of input devices used for formatting.
 */
class InputDevices {
public:
    /**
     * Create input devices.
     *
     * @param input_devices list of devices.
     */
    explicit InputDevices(std::vector<InputDevice> input_devices);

    void add_max_name_size(size_t max_name_size);
    void add_max_device_size(size_t max_device_size);
    void add_max_by_id_size(size_t max_by_id_size);
    void add_max_by_path_size(size_t max_by_path_size);

    [[nodiscard]] std::vector<InputDevice> devices() const;
    [[nodiscard]] size_t max_name_size() const;
    [[nodiscard]] size_t max_device_size() const;
    [[nodiscard]] size_t max_by_id_size() const;
    [[nodiscard]] size_t max_by_path_size() const;

private:
    size_t MIN_SPACES{1};

    std::vector<InputDevice> _devices;
    size_t _max_name_size{MIN_SPACES};
    size_t _max_device_size{MIN_SPACES};
    size_t _max_by_id_size{MIN_SPACES};
    size_t _max_by_path_size{MIN_SPACES};
};
}  // namespace ListInputDevices

template <>
struct std::formatter<ListInputDevices::InputDevices> {
public:
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    template <typename Context>
    constexpr auto format(ListInputDevices::InputDevices const& devices, Context& ctx) const {
        auto format = [&ctx, &devices](auto name, auto device, auto by_id, auto by_path, auto capabilities) {
            std::format_to(ctx.out(), "{: <{}}{: <{}}{: <{}}{: <{}}{}\n",
                name,
                devices.max_name_size(),
                device,
                devices.max_device_size(),
                by_id,
                devices.max_by_id_size(),
                by_path,
                devices.max_by_path_size(),
                capabilities
                );
        };

        format("NAME", "DEVICE", "BY_ID", "BY_PATH", "CAPABILITIES");

        for (ListInputDevices::InputDevice device : devices.devices()) {
            auto capabilities = device.getCapabilities();
            std::string capabilities_str{};
            if (!capabilities.empty()) {
                capabilities_str += "[";
                for (auto capability : device.getCapabilities()) {
                    capabilities_str += std::format("({}, {}), ", capability.first, capability.second);
                }

                capabilities_str.erase(capabilities_str.length() - 2);
                capabilities_str += "]";
            }

            format(
                device.getName().value_or(""),
                device.getDevice().string(),
                device.getById().value_or(""),
                device.getByPath().value_or(""),
                capabilities_str
                );
        }

        return std::format_to(ctx.out(), "");
    }
};

#endif  // INPUT_EVENT_RECORDER_EVENTDEVICE_H
