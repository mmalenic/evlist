#ifndef EVLIST_DEVICE_H
#define EVLIST_DEVICE_H

#include <filesystem>
#include <format>
#include <optional>
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
    [[nodiscard]] const fs::path &getDevice() const;

    /**
     * Get by id.
     * @return by id
     */
    [[nodiscard]] const std::optional<std::string> &getById() const;

    /**
     * Get by path.
     * @return by path
     */
    [[nodiscard]] const std::optional<std::string> &getByPath() const;

    /**
     * Get name.
     * @return name
     */
    [[nodiscard]] const std::optional<std::string> &getName() const;

    /**
     * Get capabilities.
     * @return capabilities
     */
    [[nodiscard]] const std::vector<std::pair<int, std::string>> &
    getCapabilities() const;

    std::partial_ordering operator<=>(const InputDevice &eventDevice) const;

private:
    fs::path device;
    std::optional<std::string> byId;
    std::optional<std::string> byPath;
    std::optional<std::string> name;
    std::vector<std::pair<int, std::string>> capabilities;

    /**
     * Partition a string into continuous segments of numbers of characters.
     */
    [[nodiscard]] static std::vector<std::string> partition(std::string str);
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
}  // namespace evlist

template <>
struct std::formatter<evlist::InputDevices> {
    static constexpr auto parse(const std::format_parse_context &ctx) {
        return ctx.begin();
    }

    template <typename Context>
    constexpr auto format(evlist::InputDevices const &devices, Context &ctx)
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
            auto capabilities = device.getCapabilities();
            std::string capabilities_str{};
            if (!capabilities.empty()) {
                capabilities_str += "[";
                for (auto [id, name] : device.getCapabilities()) {
                    capabilities_str += std::format("({}, {}), ", id, name);
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

#endif  // EVLIST_DEVICE_H
