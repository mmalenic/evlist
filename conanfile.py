from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain


class EvListRecipe(ConanFile):
    requires = "cli11/2.4.2"
    test_requires = "gtest/1.15.0"

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    options = {
        "build_testing": [True, False],
        "run_clang_tidy": [True, False],
    }
    default_options = {
        "build_testing": False,
        "run_clang_tidy": False,
    }

    def validate(self):
        if self.settings.compiler.cppstd:
            check_min_cppstd(self, 23)

    def generate(self):
        tc = CMakeToolchain(self)

        tc.variables["BUILD_TESTING"] = self.options["build_testing"]
        tc.variables["RUN_CLANG_TIDY"] = self.options["run_clang_tidy"]

        tc.generate()

    def package_info(self):
        self.conf_info.define("tools.build:compiler_executables", {"cpp": "clang++-19"})

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
