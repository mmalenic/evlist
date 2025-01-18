# List input event devices - evlist

List input event devices under `/dev/input/eventX` and format device information and capabilities.

## Usage 

List all devices and format as a table:

```sh
evlist
```

Or as a CSV:

```sh
evlist --format csv
```

The functionality of this is similar to [`libinput list-devices`][list-devices], however it can also filter on a 
specific value. Filter devices where the device name equals `device_name`:

```sh
evlist --filter name=device_name
```

This also supports filtering based on regex:

```sh
evlist --filter name=device* --use-regex
```

## Build

This project uses a [just] to manage development, [conan] as the package manager and [cmake] as the build tool.
To build the project, run:

```sh
just build
```

This requires at least [gcc-14] or [clang-19] because the project uses newer C++23 features.

## Development

Tests can be run using:

```sh
just test
```

This project uses [pre-commit] and [clang-tidy] should to lint code. To format and lint the code run:

```sh
just check
```

## Library usage

This project can also be built and used as a library. Most functionality is available from `#include <evlist/device.h>`
and `#include <evlist/list.h>`, including functions to iterate and parse `/dev/input`.

To generate function docs, run:

```sh
just doc
```

[list-devices]: https://wayland.freedesktop.org/libinput/doc/latest/tools.html#libinput-list-devices
[just]: https://github.com/casey/just
[conan]: https://docs.conan.io/2/installation.html
[cmake]: https://cmake.org/
[gcc-14]: https://gcc.gnu.org/gcc-14/
[clang-19]: https://releases.llvm.org/19.1.0/tools/clang/docs/ReleaseNotes.html
[pre-commit]: https://pre-commit.com/
