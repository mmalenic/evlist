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
    cd build/{{ capitalize(build_type) }} && ctest

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
