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

namespace evlist {

namespace fs = std::filesystem;

/**
 * Store the paths for event devices.
 */
class InputDevice {
public:
    /**
     * Create event device.
     * @param device device path
     * @param by_id by id path
     * @param by_path by path path
     * @param name device name
     * @param capabilities capabilities
     */
    InputDevice(
        fs::path device,
        std::optional<std::string> by_id,
        std::optional<std::string> by_path,
        std::optional<std::string> name,
        std::vector<std::string> capabilities
    );

    /**
     * Get device.
     * @return device
     */
    [[nodiscard]] const fs::path &device_path() const;

    /**
     * Get by id.
     * @return by id
     */
    [[nodiscard]] const std::optional<std::string> &by_id() const;

    /**
     * Get by path.
     * @return by path
     */
    [[nodiscard]] const std::optional<std::string> &by_path() const;

    /**
     * Get name.
     * @return name
     */
    [[nodiscard]] const std::optional<std::string> &name() const;

    /**
     * Get capabilities.
     * @return capabilities
     */
    [[nodiscard]] const std::vector<std::string> &capabilities() const;

    /**
     * Partition a string into continuous segments of numbers of characters.
     */
    [[nodiscard]] static std::vector<std::string> partition(std::string str);

    bool operator==(const InputDevice &other) const = default;

private:
    fs::path device_;
    std::optional<std::string> by_id_;
    std::optional<std::string> by_path_;
    std::optional<std::string> name_;
    std::vector<std::string> capabilities_;
};

/**
 * A list of input devices used for formatting.
 */
class InputDevices {
public:
    /**
     * Create input devices with the default format.
     *
     * @param input_devices list of devices.
     */
    explicit InputDevices(std::vector<InputDevice> input_devices);

    /**
     * Create an event device lister.
     */
    InputDevices(Format output_format, std::vector<InputDevice> input_devices);

    /**
     * Filter the devices.
     *
     * @param filter filter by
     * @param use_regex whether to compare using a regex
     * @return InputDevices
     */
    InputDevices &filter(
        const std::vector<std::pair<Filter, std::string>> &filter,
        bool use_regex
    );

    InputDevices &with_max_name_size(size_t max_name_size);
    InputDevices &with_max_device_size(size_t max_device_size);
    InputDevices &with_max_by_id_size(size_t max_by_id_size);
    InputDevices &with_max_by_path_size(size_t max_by_path_size);

    [[nodiscard]] std::vector<InputDevice> devices() const;
    [[nodiscard]] size_t max_name_size() const;
    [[nodiscard]] size_t max_device_size() const;
    [[nodiscard]] size_t max_by_id_size() const;
    [[nodiscard]] size_t max_by_path_size() const;

    [[nodiscard]] Format output_format() const;

private:
    static constexpr size_t MIN_SPACES{1};

    Format output_format_{Format::TABLE};
    std::vector<InputDevice> devices_;

    size_t max_name_size_{MIN_SPACES};
    size_t max_device_size_{MIN_SPACES};
    size_t max_by_id_size_{MIN_SPACES};
    size_t max_by_path_size_{MIN_SPACES};

    std::map<std::string, std::regex> regexes;

    bool filter_regex(
        const InputDevice &device, Filter filter, const std::string &value
    );
    static bool filter_equality(
        const InputDevice &device, Filter filter, const std::string &value
    );
    static bool filter_device(
        const InputDevice &device,
        Filter filter,
        std::invocable<const std::string &> auto comparison
    );
};

bool InputDevices::filter_device(
    const InputDevice &device,
    Filter filter,
    std::invocable<const std::string &> auto comparison
) {
    switch (filter) {
        case Filter::DEVICE_PATH:
            return comparison(device.device_path().c_str());
        case Filter::NAME:
            return comparison(device.name().value_or(""));
        case Filter::BY_ID:
            return comparison(device.by_id().value_or(""));
        case Filter::BY_PATH:
            return comparison(device.by_path().value_or(""));
        case Filter::CAPABILITIES:
            const auto &capabilities = device.capabilities();
            return std::ranges::any_of(
                capabilities,
                [&comparison](const auto &capability) {
                    return comparison(capability);
                }
            );
    }

    return true;
}

inline auto operator<=>(const InputDevice &lhs, const InputDevice &rhs) {
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

template <>
struct std::formatter<evlist::InputDevices> {
    static constexpr auto parse(const std::format_parse_context &ctx) {
        return ctx.begin();
    }

    template <typename Context>
    // NOLINTNEXTLINE(runtime/references)
    constexpr auto format(const evlist::InputDevices &devices, Context &ctx)
        const {
        auto csv_escape_quotes = [](const std::string &str) {
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
                          std::string name,
                          auto device,
                          auto by_id,
                          auto by_path,
                          auto capabilities
                      ) {
            switch (devices.output_format()) {
                case evlist::Format::TABLE:
                    std::format_to(
                        ctx.out(),
                        "{:<{}}{: <{}}{: <{}}{: <{}}{}\n",
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

        format("NAME", "DEVICE_PATH", "BY_ID", "BY_PATH", "CAPABILITIES");

        for (const auto &device : devices.devices()) {
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
                device.name().value_or(""),
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
