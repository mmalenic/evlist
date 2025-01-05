from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain


class EvListRecipe(ConanFile):
    requires = "cli11/2.4.2"
    test_requires = "gtest/1.15.0"

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    options = {
        # Whether to build test executables.
        "build_testing": [True, False],
        # Whether to run clang tidy.
        "run_clang_tidy": [True, False],
        # An optional clang tidy executable.
        "clang_tidy_executable": [None, "ANY"],
        # Specifies a compiler launcher, such as sccache, which sets CMAKE_<LANG>_COMPILER_LAUNCHER.
        "compiler_launcher": [None, "ANY"],
    }
    default_options = {
        "build_testing": False,
        "run_clang_tidy": False,
        "clang_tidy_executable": None,
        "compiler_launcher": None,
    }

    def validate(self):
        if self.settings.compiler.cppstd:
            check_min_cppstd(self, 23)

    def generate(self):
        tc = CMakeToolchain(self)

        tc.variables["BUILD_TESTING"] = self.options.build_testing
        tc.variables["RUN_CLANG_TIDY"] = self.options.run_clang_tidy

        if self.options.clang_tidy_executable:
            tc.variables["CLANG_TIDY_EXECUTABLE"] = self.options.clang_tidy_executable
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
