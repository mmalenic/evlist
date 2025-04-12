from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain


class EvListRecipe(ConanFile):
    name = "evlist"
    license = "MIT"
    url = "https://github.com/evlist/evlist"
    author = "Marko Malenic (mmalenic1@gmail.com)"
    description = "list input event devices"
    # x-release-please-start-version
    version = "1.0.6"
    # x-release-please-end

    requires = "cli11/2.5.0"
    test_requires = "gtest/1.16.0"

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    options = {
        # Build the binary CLI.
        "build_bin": [True, False],
        # Whether to build test executables.
        "build_testing": [True, False],
        # Whether to run clang tidy.
        "run_clang_tidy": [True, False],
        # An optional clang tidy executable.
        "clang_tidy_executable": [None, "ANY"],
        # Specifies a compiler launcher, such as sccache, which sets CMAKE_<LANG>_COMPILER_LAUNCHER.
        "compiler_launcher": [None, "ANY"],
        # Add a cmake install command after for the binary target if `build_binary` is also true:
        # https://cmake.org/cmake/help/latest/command/install.html
        "install_bin": [True, False],
        # Add a cmake install command after for the library target:
        # https://cmake.org/cmake/help/latest/command/install.html
        "install_lib": [True, False],
    }
    default_options = {
        "build_bin": True,
        "build_testing": False,
        "run_clang_tidy": False,
        "clang_tidy_executable": None,
        "compiler_launcher": None,
        "install_bin": True,
        "install_lib": True,
    }

    def validate(self):
        if self.settings.compiler.cppstd:
            check_min_cppstd(self, 23)

    def generate(self):
        tc = CMakeToolchain(self)

        tc.variables["EVLIST_BUILD_BIN"] = self.options.build_bin
        tc.variables["BUILD_TESTING"] = self.options.build_testing
        tc.variables["EVLIST_RUN_CLANG_TIDY"] = self.options.run_clang_tidy
        tc.variables["EVLIST_INSTALL_BIN"] = self.options.install_bin
        tc.variables["EVLIST_INSTALL_LIB"] = self.options.install_lib

        if self.options.clang_tidy_executable:
            tc.variables["EVLIST_CLANG_TIDY_EXECUTABLE"] = (
                self.options.clang_tidy_executable
            )
        if self.options.compiler_launcher:
            tc.variables["CMAKE_C_COMPILER_LAUNCHER"] = self.options.compiler_launcher
            tc.variables["CMAKE_CXX_COMPILER_LAUNCHER"] = self.options.compiler_launcher

        tc.generate()

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
