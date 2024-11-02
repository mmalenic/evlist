#ifndef EVLIST_LIST_H
#define EVLIST_LIST_H

#include <expected>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "device.h"

namespace evlist {
class InputDeviceLister {
public:
    /**
     * Create an event device lister.
     */
    explicit InputDeviceLister() = default;

    /**
     * list input devices.
     * @return input devices list
     */
    std::expected<InputDevices, fs::filesystem_error> listInputDevices();

private:
    fs::path inputDirectory{"/dev/input"};
    fs::path byId{inputDirectory / "by-id"};
    fs::path byPath{inputDirectory / "by-path"};
    fs::path sysClass{"/sys/class/input"};
    fs::path namePath{"device/name"};
    std::map<int, std::string> eventCodeToName{event_code_names()};

    /**
     * Check the symlink in path to see if they point to entry.
     * @param entry
     * @param path
     * @return
     */
    static std::expected<std::optional<fs::path>, fs::filesystem_error>
    checkSymlink(const fs::path &entry, const fs::path &path) noexcept;

    /**
     * Create the event codes to name map.
     * @return event codes to name map
     */
    [[nodiscard]] static std::map<int, std::string> event_code_names();

    /**
     * Get name.
     * @return name
     */
    [[nodiscard]] std::string getName(const fs::path &device);

    /**
     * Get capabilities. The underlying ioctl calls require elevated privileges.
     * @return name
     */
    [[nodiscard]] std::vector<std::pair<int, std::string>> getCapabilities(
        const fs::path &device
    ) const;
};
}  // namespace evlist

#endif  // EVLIST_LIST_H
