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
    explicit InputDeviceLister() = default;

    /**
     * list input devices.
     * @return input devices list
     */
    std::expected<InputDevices, fs::filesystem_error> listInputDevices();

private:
    std::string inputDirectory{"/dev/input"};
    std::string byId{inputDirectory + "/by-id"};
    std::string byPath{inputDirectory + "/by-path"};
    std::string sysClass{"/sys/class/input"};
    std::string namePath{"device/name"};
    std::map<uint32_t, std::string> eventCodeToName{event_code_names()};

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
    [[nodiscard]] static std::map<uint32_t, std::string> event_code_names();

    /**
     * Get name.
     * @return name
     */
    [[nodiscard]] std::string getName(const fs::path &device);

    /**
     * Get capabilities. The underlying ioctl calls require elevated privileges.
     * @return name
     */
    [[nodiscard]] std::vector<std::pair<uint32_t, std::string>> getCapabilities(
        const fs::path &device
    ) const;
};
}  // namespace evlist

#endif  // EVLIST_LIST_H
