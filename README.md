<a href="https://github.com/mmalenic/evlist/blob/main/LICENSE" target="_blank"><img
alt="MIT licensed" src="https://img.shields.io/badge/license-MIT-blue.svg"/></a>
<a href="https://github.com/mmalenic/evlist/actions?query=workflow%3Atest+branch%3Amain" target="_blank"><img
alt="Build status" src="https://github.com/mmalenic/cmake-toolbelt/actions/workflows/test.yaml/badge.svg"/></a>
<a href="https://mmalenic.github.io/evlist" target="_blank"><img
alt="MIT licensed" src="https://img.shields.io/badge/docs-API-blue.svg"/></a>

# List input event devices - evlist

[API Documentation][api-docs]

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

> [!NOTE]
> Viewing and filtering capabilities requires elevated privileges.

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

This project uses [pre-commit] and [clang-tidy] to lint code. To format and lint the code run:

```sh
just check
```

## Library usage

This project can also be built and used as a library. Functionality is available from `#include <evlist/evlist.h>`,
including functions to iterate and parse `/dev/input`. View the API docs [here][api-docs].

The library can be built without the binary using:

```sh
just build_lib
```

If using this project as a submodule and cmake, it can be included by using `add_subdirectory` and linking against the
`libevlist` target:

```cmake
add_subdirectory(evlist)
target_link_libraries(${PROJECT_NAME} PRIVATE libevlist)
```

Code documentation can be generated by running:

```sh
just doc
```

[list-devices]: https://wayland.freedesktop.org/libinput/doc/latest/tools.html#libinput-list-devices
[just]: https://github.com/casey/just
[conan]: https://docs.conan.io/2/installation.html
[cmake]: https://cmake.org/
[gcc-14]: https://gcc.gnu.org/gcc-14/
[clang-19]: https://releases.llvm.org/19.1.0/tools/clang/docs/ReleaseNotes.html
[clang-tidy]: https://clang.llvm.org/extra/clang-tidy/
[pre-commit]: https://pre-commit.com/
[api-docs]: https://mmalenic.github.io/evlist
