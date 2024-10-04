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
test build_type='Debug' *options='': (build build_type options)
    cd build/{{ capitalize(build_type) }} && ctest