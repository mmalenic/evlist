/**
 * @file list.h
 *
 * Contains definitions for listing input devices on the system.
 */

#ifndef EVLIST_LIST_H
#define EVLIST_LIST_H

#include <expected>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "evlist/device.h"

/**
 * The namespace for this project.
 */
namespace evlist {

/**
 * List all input devices on the system.
 */
class InputDeviceLister {
public:
    /**
     * Create an event device lister with default values.
     */
    InputDeviceLister() = default;

    /**
     * Create an event device lister.
     *
     * @param output_format output format to
     * @param use_regex whether to compare filters using regex
     * @param filter filter output devices by
     */
    InputDeviceLister(
        Format output_format,
        bool use_regex,
        std::vector<std::pair<Filter, std::string>> filter
    );

    /**
     * List all input devices on the system, applying filters if they
     * were specified.
     *
     * @return the input devices list or a filesystem error if any error
     *         occurred
     */
    [[nodiscard]] std::expected<InputDevices, fs::filesystem_error>
    list_input_devices() const;

private:
    Format output_format_{Format::TABLE};
    bool use_regex_{false};
    std::vector<std::pair<Filter, std::string>> filter_;

    std::string input_directory_{"/dev/input"};
    std::string by_id_{input_directory_ + "/by-id"};
    std::string by_path_{input_directory_ + "/by-path"};
    std::string sys_class_{"/sys/class/input"};
    std::string name_path_{"device/name"};
    std::map<uint32_t, std::string> event_names_{event_names()};

    static std::expected<std::optional<fs::path>, fs::filesystem_error>
    check_symlink(const fs::path& entry, const fs::path& path) noexcept;
    [[nodiscard]] static std::map<uint32_t, std::string> event_names();

    [[nodiscard]] std::string name(const fs::path& device) const;
    [[nodiscard]] std::vector<std::string> capabilities(
        const fs::path& device
    ) const;
};
} // namespace evlist

#endif // EVLIST_LIST_H
