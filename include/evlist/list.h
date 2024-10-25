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
    InputDeviceLister();

    /**
     * list input devices.
     * @return input devices list
     */
    std::expected<InputDevices, fs::filesystem_error> listInputDevices();

    /**
     * Get event devices.
     * @return event devices
     */
    [[nodiscard]] const InputDevices &getInputDevices() const;

private:
    fs::path inputDirectory;
    fs::path byId;
    fs::path byPath;
    fs::path sysClass;
    fs::path namePath;
    std::map<int, std::string> eventCodeToName;

    size_t maxNameSize{0};
    size_t maxPathSize{0};
    InputDevices inputDevices;

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
    [[nodiscard]] static std::map<int, std::string> getEventCodeToName();

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
