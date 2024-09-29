profile:
    conan profile detect -e

build build_type='Debug': profile
    conan build . -s build_type={{build_type}} -s compiler.cppstd=23 --build=missing