# Detect the conan profile for building.
profile:
    conan profile detect -e

# Build evlist, optionally building tests.
build build_type='Debug' *options='': profile
    conan build . -s build_type={{ capitalize(build_type) }} -s compiler.cppstd=23 {{options}} --build=missing

# Build and run evlist.
run build_type='Debug' *options='': (build build_type options)
    cd build/{{ capitalize(build_type) }} && ./evlist

# Build and test evlist.
test build_type='Debug' *options='-o build_testing=True': (build build_type options)
    cd build/{{ capitalize(build_type) }} && ctest

# Run pre-commit and other lints.
lint:
    pre-commit run --all-files

# Run clang tidy on code.
check build_type='Debug' *options='-o build_testing=True -o run_clang_tidy=True': lint (build build_type options)

# Remove the build directory.
clean:
    rm -r build
