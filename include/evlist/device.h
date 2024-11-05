#ifndef EVLIST_DEVICE_H
#define EVLIST_DEVICE_H

#include <filesystem>
#include <format>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

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
        std::vector<std::pair<uint32_t, std::string>> capabilities
    );

    /**
     * Get device.
     * @return device
     */
    [[nodiscard]] const fs::path &device() const;

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
    [[nodiscard]] const std::vector<std::pair<uint32_t, std::string>> &
    capabilities() const;

    /**
     * Partition a string into continuous segments of numbers of characters.
     */
    [[nodiscard]] static std::vector<std::string> partition(std::string str);

private:
    fs::path device_;
    std::optional<std::string> by_id_;
    std::optional<std::string> by_path_;
    std::optional<std::string> name_;
    std::vector<std::pair<uint32_t, std::string>> capabilities_;
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

    InputDevices &with_max_name_size(size_t max_name_size);
    InputDevices &with_max_device_size(size_t max_device_size);
    InputDevices &with_max_by_id_size(size_t max_by_id_size);
    InputDevices &with_max_by_path_size(size_t max_by_path_size);

    [[nodiscard]] std::vector<InputDevice> devices() const;
    [[nodiscard]] size_t max_name_size() const;
    [[nodiscard]] size_t max_device_size() const;
    [[nodiscard]] size_t max_by_id_size() const;
    [[nodiscard]] size_t max_by_path_size() const;

private:
    size_t MIN_SPACES{1};

    std::vector<InputDevice> devices_;
    size_t max_name_size_{MIN_SPACES};
    size_t max_device_size_{MIN_SPACES};
    size_t max_by_id_size_{MIN_SPACES};
    size_t max_by_path_size_{MIN_SPACES};
};

inline auto operator<=>(const InputDevice &lhs, const InputDevice &rhs) {
    auto lhs_device = lhs.device().string();
    auto rhs_device = rhs.device().string();

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
        auto format = [&ctx, &devices](
                          auto name,
                          auto device,
                          auto by_id,
                          auto by_path,
                          auto capabilities
                      ) {
            std::format_to(
                ctx.out(),
                "{: <{}}{: <{}}{: <{}}{: <{}}{}\n",
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

        for (evlist::InputDevice const &device : devices.devices()) {
            auto capabilities = device.capabilities();
            std::string capabilities_str{};
            if (!capabilities.empty()) {
                capabilities_str += "[";
                for (auto [id, name] : device.capabilities()) {
                    capabilities_str += std::format("({}, {}), ", id, name);
                }

                capabilities_str.erase(capabilities_str.length() - 2);
                capabilities_str += "]";
            }

            format(
                device.name().value_or(""),
                device.device().string(),
                device.by_id().value_or(""),
                device.by_path().value_or(""),
                capabilities_str
            );
        }

        return std::format_to(ctx.out(), "");
    }
};

#endif // EVLIST_DEVICE_H
