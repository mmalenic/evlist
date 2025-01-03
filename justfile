# Detect the conan profile for building.
profile:
    conan profile detect -e

# Update the lock file.
update:
    conan lock create .

# Build evlist.
build build_type='Debug' *build_options='': profile clean-cache
    conan build . -s build_type={{ capitalize(build_type) }} -s compiler.cppstd=23 {{build_options}} --build=missing

# Rebuild evlist using the existing CMake directory.
rebuild build_type='Debug':
    cd build/{{ capitalize(build_type) }} && cmake --build .

# Build and run evlist.
run args='--help' build_type='Release' *build_options='': (build build_type build_options)
    cd build/{{ capitalize(build_type) }} && ./evlist {{args}}

# Build and test evlist.
test build_type='Debug' *build_options='-o build_testing=True': (build build_type build_options)
    cd build/{{ capitalize(build_type) }} && ./evlisttest --gtest_filter=-InputDeviceLister*

# Build an test evlist integration tests
test-integration build_type='Debug' *build_options='-o build_testing=True': (build build_type build_options)
    cd build/{{ capitalize(build_type) }} && sudo ./evlisttest --gtest_filter=InputDeviceLister*

# Default valgrind command
valgrind := "valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=1"

# Build and test evlist using address sanitization and valgrind memcheck
memcheck build_type='Debug' *build_options='': (build build_type build_options)
    cd build/{{ capitalize(build_type) }} && \
    {{ valgrind }} \
    ./evlisttest --gtest_filter=-InputDeviceLister*

# Build and test evlist integration tests
integration-memcheck build_type='Debug' *build_options='': (build build_type build_options)
    cd build/{{ capitalize(build_type) }} && \
    sudo {{ valgrind }} \
    ./evlisttest --gtest_filter=InputDeviceLister*

# Run pre-commit and other lints.
lint:
    pre-commit run --all-files

# Run clang tidy on code.
check build_type='Debug' *build_options='-o build_testing=True -o run_clang_tidy=True': lint (build build_type build_options)

# Remove the build directory.
clean:
    rm -rf build

# Clean the CMakeCache.txt only.
clean-cache:
    rm -f build/Debug/CMakeCache.txt && rm -f build/Release/CMakeCache.txt

# Generate documentation.
doc:
    git clone --depth 1 --branch v2.3.4 https://github.com/jothepro/doxygen-awesome-css.git build/doxygen-awesome-css || true
    doxygen

# Fetch the llvm repo for building sanitized binaries.
fetch-llvm clang_version='19':
    #!/usr/bin/env bash
    set -euxo pipefail

    if [ -d "llvm-project" ]; then
        echo "llvm already fetched - skipping"
    else
        version=$(clang-{{ clang_version }} --version | grep -Po '(\d+).(\d+).(\d+)' | head -1)
        git clone --depth 1 --branch llvmorg-"$version" https://github.com/llvm/llvm-project.git
    fi

# Build llvm with options
build-llvm dir runtimes use_sanitizer targets clang_version='19': (fetch-llvm clang_version)
    #!/usr/bin/env bash
    set -euxo pipefail

    cd llvm-project

    if [ -d "{{ dir }}" ]; then
        echo "llvm at {{ dir }} already built - skipping"
    else
        CC=clang-{{ clang_version }} CXX=clang++-{{ clang_version }} \
            cmake -G Ninja -S runtimes -B {{ dir }} \
            -DCMAKE_BUILD_TYPE=Debug \
            -DLLVM_ENABLE_RUNTIMES="{{ runtimes }}" \
            -DLLVM_USE_SANITIZER="{{ use_sanitizer }}"
        ninja -C {{ dir }} {{ targets }}
    fi

build-with-clang flags clang_version='19':
    #!/usr/bin/env bash
    set -euxo pipefail

    just build Debug -o build_testing=True --build=* \
        -s compiler=clang \
        -s compiler.version={{ clang_version }} \
        -c tools.build:cxxflags="\"{{ flags }}\"" \
        -c tools.build:cflags="\"{{ flags }}\"" \
        -c tools.build:compiler_executables="\"{'c':'clang-{{ clang_version }}','cpp':'clang++-{{ clang_version }}'}\""

# Create the ignorelist for sanitizer tests to ignore non-project code.
create-ignore-list dir:
    build_dir=$(realpath build/Debug) && mkdir -p build_dir && echo "src:{{ dir }}/*" >> $build_dir/ignorelist.txt

# Build llvm cxx and cxxabi with the memory sanitizer
build-msan clang_version='19': (build-llvm "build-msan" "libcxx;libcxxabi;libunwind" "MemoryWithOrigins" "cxx cxxabi" clang_version)

# Run tests with the memory sanitizer
test-msan filter='-InputDeviceLister*' clang_version='19': (build-msan clang_version)
    #!/usr/bin/env bash
    set -euxo pipefail

    dir=$(realpath llvm-project/build-msan)
    just create-ignore-list "$dir"

    flags="[\
        '-fsanitize=memory','-nostdinc++','-stdlib=libc++','-lc++abi','-fno-omit-frame-pointer',\
        '-L$dir/lib','-I$dir/include','-I$dir/include/c++/v1','-Wl,--rpath=$dir/lib','-fsanitize-ignorelist=$(realpath build/Debug)/ignorelist.txt'\
    ]"

    just build-with-clang "$flags" {{ clang_version }}

    cd build/Debug && MSAN_SYMBOLIZER_PATH=llvm-symbolizer-{{ clang_version }} ./evlisttest --gtest_filter={{ filter }}

# Remove the built msan directory.
clean-msan: clean-cache
    rm -rf llvm-project/build-msan

# Build llvm cxx and cxxabi with the address sanitizer
build-asan clang_version='19': (build-llvm "build-asan" "libcxx;libcxxabi;libunwind" "Address;Undefined" "cxx cxxabi" clang_version)

# Run tests with the address sanitizer
test-asan filter='-InputDeviceLister*' clang_version='19': (build-msan clang_version)
    #!/usr/bin/env bash
    set -euxo pipefail

    dir=$(realpath llvm-project/build-asan)
    just create-ignore-list "$dir"

    flags="[\
        '-fsanitize=address,undefined,leak,integer','-nostdinc++','-stdlib=libc++','-lc++abi',\
        '-L$dir/lib','-I$dir/include','-I$dir/include/c++/v1','-Wl,--rpath=$dir/lib','-fsanitize-ignorelist=$(realpath build/Debug)/ignorelist.txt'\
    ]"

    just build-with-clang "$flags" {{ clang_version }}

    cd build/Debug && ASAN_SYMBOLIZER_PATH=llvm-symbolizer-{{ clang_version }} ./evlisttest --gtest_filter={{ filter }}

# Remove the built asan directory.
clean-asan: clean-cache
    rm -rf llvm-project/build-asan
