#ifndef EVLIST_LIST_H
#define EVLIST_LIST_H

#include <expected>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "evlist/device.h"

namespace evlist {
class InputDeviceLister {
public:
    /**
     * Create an event device lister.
     */
    InputDeviceLister() = default;

    /**
     * Create an event device lister.
     */
    explicit InputDeviceLister(Format output_format);

    /**
     * list input devices.
     * @return input devices list
     */
    std::expected<InputDevices, fs::filesystem_error> list_input_devices();

private:
    Format output_format_{Format::TABLE};

    std::string input_directory_{"/dev/input"};
    std::string by_id_{input_directory_ + "/by-id"};
    std::string by_path_{input_directory_ + "/by-path"};
    std::string sys_class_{"/sys/class/input"};
    std::string name_path_{"device/name"};
    std::map<uint32_t, std::string> event_names_{event_names()};

    /**
     * Check the symlink in path to see if they point to entry.
     * @param entry
     * @param path
     * @return
     */
    static std::expected<std::optional<fs::path>, fs::filesystem_error>
    check_symlink(const fs::path &entry, const fs::path &path) noexcept;

    /**
     * Create the event codes to name map.
     * @return event codes to name map
     */
    [[nodiscard]] static std::map<uint32_t, std::string> event_names();

    /**
     * Get name.
     * @return name
     */
    [[nodiscard]] std::string name(const fs::path &device) const;

    /**
     * Get capabilities. The underlying ioctl calls require elevated privileges.
     * @return name
     */
    [[nodiscard]] std::vector<std::string> capabilities(const fs::path &device
    ) const;
};
} // namespace evlist

#endif // EVLIST_LIST_H
