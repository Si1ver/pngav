# PNG Alpha Viewer: List of Things To Do

This document contains a list of planned changes.

The list reflects personal interests and is not necessarily based on the project's functional development needs. Before implementing any of the changes listed below, consider whether they **align with your goals or values**.

There is no guarantee that any of these changes will actually be implemented.

## Features

*No plans at the moment.*

## Source Code

* Perform general cleanup of the source code and ensure it compiles with modern version of Visual Studio.

* Remove support for obsolete or unsupported compilers.

* Compile the code with [warnings and strict standards conformance](https://github.com/cpp-best-practices/cppbestpractices/blob/master/02-Use_the_Tools_Available.md#msvc).

* Investigate how much effort is needed to make this tool support more platforms, namely Linux and macOS.

## Documentation

* Add license information according to original license.

* Add building instructions.

* Add usage documentation based on original docs.

* Add a changelog file with a [curated, chronological list of notable changes for each version of the project](https://keepachangelog.com/en/1.1.0/).

* Document [Susie](https://wiki.multimedia.cx/index.php/Susie) plugins feature.

## Building and Automation

* Add CMake build scripts for the project and configure them to use vcpkg for dependency management.

* Set up a CI workflow to build the project automatically before merging pull requests.
