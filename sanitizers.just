# Test evlist using address and memory sanitizers. This needs to compile libcxx and libcxxabi with sanitiziers enabled.

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

    just build Debug -o build_testing=True \
        -s compiler=clang \
        -s compiler.version={{ clang_version }} \
        -c tools.build:cxxflags="\"{{ flags }}\"" \
        -c tools.build:cflags="\"{{ flags }}\"" \
        -c tools.build:compiler_executables="\"{'c':'clang-{{ clang_version }}','cpp':'clang++-{{ clang_version }}'}\""

# Create the ignorelist for sanitizer tests to ignore non-project code.
create-ignore-list dir:
    mkdir -p build/Debug && echo "src:{{ dir }}/*" >> build/Debug/ignorelist.txt

# Error if the log file from sanitizers is non-empty.
error-non-empty-log:
    #!/usr/bin/env bash
    set -euxo pipefail

    if [ -f log ] && [ -s log ]; then
        cat log
        exit 1
    fi

# Test using an llvm sanitizer build directory.
test-llvm dir extra_flags symbolizer_path filter='*' clang_version='19':
    #!/usr/bin/env bash
    set -euxo pipefail

    dir=$(realpath llvm-project/{{ dir }})
    just create-ignore-list "$dir"

    flags="[\
        '-nostdinc++','-stdlib=libc++','-lc++abi','-L$dir/lib','-I$dir/include','-I$dir/include/c++/v1',\
        '-Wl,--rpath=$dir/lib','-fsanitize-ignorelist=$(realpath build/Debug)/ignorelist.txt',{{ extra_flags }}\
    ]"

    just build-with-clang "$flags" {{ clang_version }}

    cd build/Debug && {{ symbolizer_path }}=llvm-symbolizer-{{ clang_version }} ./evlisttest --gtest_filter={{ filter }} 2> log
    just error-non-empty-log

# Run tests with the memory sanitizer
test-valgrind filter='*': (build 'Debug' '-o build_testing=True')
    cd build/Debug && \
        valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=1  \
        ./evlisttest --gtest_filter={{ filter }}

# Build llvm cxx and cxxabi with the memory sanitizer
build-msan clang_version='19': (build-llvm "build-msan" "libcxx;libcxxabi;libunwind" "MemoryWithOrigins" "cxx cxxabi" clang_version)

# Run tests with the memory sanitizer
test-msan filter='*' clang_version='19': (build-msan clang_version) \
    (test-llvm "build-msan" "'-fsanitize=memory','-fno-omit-frame-pointer'" "MSAN_SYMBOLIZER_PATH" filter clang_version)

# Remove the built msan directory.
clean-msan: clean-cache
    rm -rf llvm-project/build-msan

# Build llvm cxx and cxxabi with the address sanitizer
build-asan clang_version='19': (build-llvm "build-asan" "libcxx;libcxxabi;libunwind" "Address;Undefined" "cxx cxxabi" clang_version)

# Run tests with the address sanitizer
test-asan filter='*' clang_version='19': (build-msan clang_version) \
    (test-llvm "build-asan" "'-fsanitize=address,undefined,leak,integer'" "ASAN_SYMBOLIZER_PATH" filter clang_version)

# Remove the built asan directory.
clean-asan: clean-cache
    rm -rf llvm-project/build-asan
