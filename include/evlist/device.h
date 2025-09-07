/**
 * @file device.h
 *
 * Contains definitions for input device containers.
 */

#ifndef EVLIST_DEVICE_H
#define EVLIST_DEVICE_H

#include <filesystem>
#include <format>
#include <map>
#include <optional>
#include <ranges>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "evlist/cli.h"

/**
 * The namespace for this project.
 */
namespace evlist {

namespace fs = std::filesystem;

/**
 * Store data such as paths and names of input event devices.
 */
class InputDevice {
public:
    /**
     * Create the input event device.
     *
     * @param device the device path under `/dev/input/`
     * @param by_id the by-id path under `/dev/input/by-id/`
     * @param by_path by-path path under `/dev/input/by-path/`
     * @param name the device name
     * @param capabilities the device capabilities obtained by querying
     *        [ioctl using
     * EVIOCGBIT](https://www.kernel.org/doc/html/latest/input/ff.html#querying-device-capabilities).
     */
    InputDevice(
        fs::path device,
        std::string name,
        std::optional<std::string> by_id,
        std::optional<std::string> by_path,
        std::vector<std::string> capabilities
    );

    /**
     * Get the device.
     *
     * @return device
     */
    [[nodiscard]] const fs::path& device_path() const;

    /**
     * Get the by-id path.
     *
     * @return by-id path
     */
    [[nodiscard]] const std::optional<std::string>& by_id() const;

    /**
     * Get the by-path path.
     *
     * @return by-path path
     */
    [[nodiscard]] const std::optional<std::string>& by_path() const;

    /**
     * Get the name.
     *
     * @return name
     */
    [[nodiscard]] const std::string& name() const;

    /**
     * Get the capabilities.
     *
     * @return capabilities
     */
    [[nodiscard]] const std::vector<std::string>& capabilities() const;

    /**
     * Partition a string into segments of numbers and characters, where
     * continuous numbers are part of the same partition. This is used to
     * support natural sorting of `evlist::InputDevices`.
     *
     * @param str input string
     * @return a vector of partitioned strings
     */
    [[nodiscard]] static std::vector<std::string> partition(std::string str);

    /**
     * Compare equality by each field in the input device.
     *
     * @param other compare to
     * @return whether devices are equal
     */
    bool operator==(const InputDevice& other) const = default;

private:
    fs::path device_;
    std::string name_;
    std::optional<std::string> by_id_;
    std::optional<std::string> by_path_;
    std::vector<std::string> capabilities_;
};

/**
 * A list of `evlist::InputDevice`.
 */
class InputDevices {
public:
    /**
     * The name of the header for the device name.
     */
    static constexpr std::string_view HEADER_NAME = "NAME";

    /**s
     * The name of the header for the device path.
     */
    static constexpr std::string_view HEADER_DEVICE_PATH = "DEVICE_PATH";

    /**
     * The name of the header for the by-id path.
     */
    static constexpr std::string_view HEADER_BY_ID = "BY_ID";

    /**
     * The name of the header for the by-path path.
     */
    static constexpr std::string_view HEADER_BY_PATH = "BY_PATH";

    /**
     * The name of the header for the capabilities.
     */
    static constexpr std::string_view HEADER_CAPABILITIES = "CAPABILITIES";

    /**
     * Create input devices with the default format.
     *
     * @param input_devices list of devices
     */
    explicit InputDevices(std::vector<InputDevice> input_devices);

    /**
     * Create input devices.
     *
     * @param output_format the output format
     * @param input_devices list of devices
     */
    InputDevices(Format output_format, std::vector<InputDevice> input_devices);

    /**
     * Filter the devices so that only devices matching the filter remain.
     *
     * @param filter filter by a vector of pairs of the filter type and string.
     *        This will only include devices that equal the string for a given
     *        filter. Capabilities include devices where all the capabilities
     *        match the filters.
     * @param use_regex whether to compare using a regex instead of equality
     * @return filtered input devices
     */
    InputDevices& filter(
        const std::vector<std::pair<Filter, std::string>>& filter,
        bool use_regex
    );

    /**
     * Set the maximum length of the device name field used for formatting
     * `evlist::Format` table output.
     *
     * @param max_name_size new max name size
     * @return this instance of `InputDevices`
     */
    InputDevices& with_max_name(size_t max_name_size);

    /**
     * Set the maximum length of the device path field used for formatting
     * `evlist::Format` table output.
     *
     * @param max_device_size new max device size
     * @return this instance of `InputDevices`
     */
    InputDevices& with_max_device_path(size_t max_device_size);

    /**
     * Set the maximum length of the by-id field used for formatting
     * `evlist::Format` table output.
     *
     * @param max_by_id_size new max device size
     * @return this instance of `InputDevices`
     */
    InputDevices& with_max_by_id(size_t max_by_id_size);

    /**
     * Set the maximum length of the by-path field used for formatting
     * `evlist::Format` table output.
     *
     * @param max_by_path_size new max device size
     * @return this instance of `InputDevices`
     */
    InputDevices& with_max_by_path(size_t max_by_path_size);

    /**
     * Set the underlying input devices.
     *
     * @param input_devices new input devices
     * @return this instance of `InputDevices`
     */
    InputDevices& with_input_devices(std::vector<InputDevice> input_devices);

    /**
     * Get the input devices.
     *
     * @return input devices
     */
    [[nodiscard]] std::vector<InputDevice> devices() const;

    /**
     * Get the maximum length of the device name field used for formatting
     * `evlist::Format` table output.
     *
     * @return max length of device name field
     */
    [[nodiscard]] size_t max_name() const;

    /**
     * Get the maximum length of the device name field used for formatting
     * `evlist::Format` table output.
     *
     * @return max length of device path field
     */
    [[nodiscard]] size_t max_device_path() const;

    /**
     * Get the maximum length of the by-id field used for formatting
     * `evlist::Format` table output.
     *
     * @return max length of by-id field
     */
    [[nodiscard]] size_t max_by_id() const;

    /**
     * Get the maximum length of the by-path field used for formatting
     * `evlist::Format` table output.
     *
     * @return max length of by-path field
     */
    [[nodiscard]] size_t max_by_path() const;

    /**
     * Get the output format.
     *
     * @return output format
     */
    [[nodiscard]] Format output_format() const;

private:
    Format output_format_{Format::TABLE};
    std::vector<InputDevice> devices_;

    std::size_t MIN_SPACES{1};
    std::size_t max_name_size_{HEADER_NAME.length() + MIN_SPACES};
    std::size_t max_device_size_{HEADER_DEVICE_PATH.length() + MIN_SPACES};
    std::size_t max_by_id_size_{HEADER_BY_ID.length() + MIN_SPACES};
    std::size_t max_by_path_size_{HEADER_BY_PATH.length() + MIN_SPACES};

    std::map<std::string, std::regex> regexes;

    bool filter_regex(
        const InputDevice& device, Filter filter, const std::string& value
    );
    static bool filter_equality(
        const InputDevice& device, Filter filter, const std::string& value
    );
    static bool filter_device(
        const InputDevice& device,
        Filter filter,
        std::invocable<const std::string&> auto comparison
    );
};

bool InputDevices::filter_device(
    const InputDevice& device,
    Filter filter,
    std::invocable<const std::string&> auto comparison
) {
    switch (filter) {
        case Filter::DEVICE_PATH:
            return comparison(device.device_path().c_str());
        case Filter::NAME:
            return comparison(device.name());
        case Filter::BY_ID:
            return comparison(device.by_id().value_or(""));
        case Filter::BY_PATH:
            return comparison(device.by_path().value_or(""));
        case Filter::CAPABILITIES:
            const auto& capabilities = device.capabilities();
            return std::ranges::any_of(
                capabilities, [&comparison](const auto& capability) {
                    return comparison(capability);
                }
            );
    }

    return true;
}

/**
 * Compare two input devices using natural sorting where multi-digit numbers
 * are treated as a single number.
 *
 * @param lhs compare with left input device
 * @param rhs compare with right input device
 * @return the
 * [`std::strong_ordering`](https://en.cppreference.com/w/cpp/utility/compare/strong_ordering)
 *         sort order
 */
inline auto operator<=>(const InputDevice& lhs, const InputDevice& rhs) {
    auto lhs_device = lhs.device_path().string();
    auto rhs_device = rhs.device_path().string();

    auto s1_partitions = InputDevice::partition(lhs_device);
    auto s2_partitions = InputDevice::partition(rhs_device);

    for (auto [lhs, rhs] : std::views::zip(s1_partitions, s2_partitions)) {
        if (lhs != rhs && !lhs.empty() && !rhs.empty()) {
            if (std::isdigit(lhs[0]) != 0 && std::isdigit(rhs[0]) != 0) {
                return std::stoi(lhs) <=> std::stoi(rhs);
            }
            return lhs <=> rhs;
        }
    }

    return lhs_device <=> rhs_device;
}

} // namespace evlist

/**
 * Defines the
 * [`std:formatter`](https://en.cppreference.com/w/cpp/utility/format/formatter)
 * for formatting `evlist::InputDevices`.
 */
template <>
struct std::formatter<evlist::InputDevices> {
    /**
     * Parse the input devices by beginning a new iterator from the context.
     *
     * @param ctx formatting context
     * @return output iterator
     */
    static constexpr auto parse(const std::format_parse_context& ctx) {
        return ctx.begin();
    }

    /**
     * Format the input devices based on the output `Format`.
     *
     * @tparam Context context type
     * @param devices input devices
     * @param ctx context parameter
     * @return iterator after formatting
     */
    template <typename Context>
    // NOLINTNEXTLINE(runtime/references)
    constexpr auto format(
        const evlist::InputDevices& devices, Context& ctx
    ) const {
        auto csv_escape_quotes = [](std::string_view str) {
            std::string out = {};
            out.reserve(str.length());

            for (const auto character : str) {
                if (character == '"') {
                    out.push_back('"');
                }
                out.push_back(character);
            }

            return out;
        };

        auto format = [&csv_escape_quotes, &ctx, &devices](
                          std::string_view name,
                          std::string_view device,
                          std::string_view by_id,
                          std::string_view by_path,
                          std::string_view capabilities
                      ) {
            switch (devices.output_format()) {
                case evlist::Format::TABLE:
                    std::format_to(
                        ctx.out(),
                        "{:<{}}{: <{}}{: <{}}{: <{}}{}\n",
                        name,
                        devices.max_name(),
                        device,
                        devices.max_device_path(),
                        by_id,
                        devices.max_by_id(),
                        by_path,
                        devices.max_by_path(),
                        capabilities
                    );
                    break;
                case evlist::Format::CSV:
                    std::format_to(
                        ctx.out(),
                        R"("{}","{}","{}","{}","{}")"
                        "\n",
                        csv_escape_quotes(name),
                        csv_escape_quotes(device),
                        csv_escape_quotes(by_id),
                        csv_escape_quotes(by_path),
                        csv_escape_quotes(capabilities)
                    );
                    break;
            }
        };

        format(
            evlist::InputDevices::HEADER_NAME,
            evlist::InputDevices::HEADER_DEVICE_PATH,
            evlist::InputDevices::HEADER_BY_ID,
            evlist::InputDevices::HEADER_BY_PATH,
            evlist::InputDevices::HEADER_CAPABILITIES
        );

        for (const auto& device : devices.devices()) {
            auto capabilities = device.capabilities();
            std::string capabilities_str{};
            if (!capabilities.empty()) {
                capabilities_str += "[";
                for (auto name : device.capabilities()) {
                    capabilities_str += std::format("{}, ", name);
                }

                capabilities_str.erase(capabilities_str.length() - 2);
                capabilities_str += "]";
            }

            format(
                device.name(),
                device.device_path().string(),
                device.by_id().value_or(""),
                device.by_path().value_or(""),
                capabilities_str
            );
        }

        return std::format_to(ctx.out(), "");
    }
};

#endif // EVLIST_DEVICE_H
