# PNG Alpha Viewer: Building Instructions

## Prerequisites

To build the tool from source, install the following:

* `Visual Studio 2022` with MSVC compiler *(choose `Desktop development with C++` workload for easy installation of required components)*;
* `CMake 3.21` *or newer*;
* `Git` *(for installing vcpkg)*.

## Preparing Environment

1. Install vcpkg

    vcpkg is a free and open-source C/C++ package manager used to provide required third-party libraries.

    vcpkg can be installed in any directory. The recommended approach is to install vcpkg to the root directory of this project.

    To install vcpkg, open a [terminal](https://learn.microsoft.com/en-us/windows/terminal/) (or a command prompt) in the desired directory and execute the following commands one by one:

    ```cmd
    git clone https://github.com/microsoft/vcpkg.git
    vcpkg\bootstrap-vcpkg.bat
    set VCPKG_ROOT=%cd%\vcpkg
    set PATH=%PATH%;%VCPKG_ROOT%;
    ```

    **Important:** Run CMake from the same terminal session where the environment variables (`VCPKG_ROOT` and `PATH`) were set.

    More information about installing vcpkg can be found [in the documentation](https://learn.microsoft.com/en-gb/vcpkg/get_started/get-started?pivots=shell-cmd).

## Building

1. Generate Visual Studio solution

    CMake configuration presets for common build configurations are defined in the `CMakePresets.json` file.

    Execute the following command to generate a Visual Studio solution using CMake:

    ```cmd
    cmake -S <source path> -B <build path> --preset <preset name>
    ```

    In this command, the following fields should be specified:

    * `<source path>` is the path to the root directory of the project.
    * `<build path>` is the path to the directory where solution files will be generated.
    * `<preset name>` is one of the predefined configuration presets (see the list below).

    | Preset Name | Platform | Configurations | Generator |
    | ----------- | -------- | -------------- | --------- |
    | `windows-x64-vs2022` | Windows, x86-64 (64-bit) | Release, Release with Debug Info, Debug | Visual Studio 2022 |
    | `windows-x86-vs2022` | Windows, x86 (32-bit) | Release, Release with Debug Info, Debug | Visual Studio 2022 |
    | `windows-arm64-vs2022` | Windows, AArch64 (64-bit) | Release, Release with Debug Info, Debug | Visual Studio 2022 |

    Example command when executed from the root directory of the project:

    ```cmd
    cmake -S . -B build --preset windows-x64-vs2022
    ```

    Generated files will appear in the specified `<build path>` directory.

2. Build the tool executable

    CMake build presets for common build configurations are defined in the `CMakePresets.json` file.

    Execute the following command to build the generated solution using CMake:

    ```cmd
    cmake --build <build path> --preset <preset name>
    ```

    In this command, the following fields should be specified:

    * `<build path>` is the path to the directory where solution files were generated.
    * `<preset name>` is one of the predefined build presets (see the list below).

    | Build Preset Name | Corresponding Configuration Preset | Platform | Configuration |
    | ----------------- | ---------------------------------- | -------- | ------------- |
    | `windows-x64-vs2022-release` | `windows-x64-vs2022` | Windows, x86-64 (64-bit) | Release |
    | `windows-x64-vs2022-debug` | `windows-x64-vs2022` | Windows, x86-64 (64-bit) | Debug |
    | `windows-x86-vs2022-release` | `windows-x86-vs2022` | Windows, x86 (32-bit) | Release |
    | `windows-x86-vs2022-debug` | `windows-x86-vs2022` | Windows, x86 (32-bit) | Debug |
    | `windows-arm64-vs2022-release` | `windows-arm64-vs2022` | Windows, AArch64 (64-bit) | Release |
    | `windows-arm64-vs2022-debug` | `windows-arm64-vs2022` | Windows, AArch64 (64-bit) | Debug |

    Example command when executed from the root directory of the project:

    ```cmd
    cmake --build build --preset windows-x64-vs2022-release
    ```

    > Note: The generated solution can be built using Visual Studio. \
    > Open the `pngav.sln` solution file in Visual Studio and select `Build > Build Solution` from the menu to produce an executable application.
