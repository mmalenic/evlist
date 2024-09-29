# Detect the conan profile for building.
profile:
    conan profile detect -e

# Build evlist, optionally building tests.
build build_type='Debug' *options='': profile
    conan build . -s build_type={{ capitalize(build_type) }} -s compiler.cppstd=23 {{options}} --build=missing
