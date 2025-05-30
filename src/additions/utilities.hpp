// This source file was added to the original project to extend or customize functionality.
// It is licensed under the same terms as the original project.
//
// For license details, see the LICENSE.md or COPYING.txt file in the root of the repository.
//
// Author: @Si1ver, 2025.

#pragma once

#ifndef ADDITIONS_UTILITIES_HPP
#define ADDITIONS_UTILITIES_HPP

#include <filesystem>

namespace additions {

class Utilities {
public:
  static std::filesystem::path getExecutablePath();
};

}  // namespace additions

#endif  // ADDITIONS_UTILITIES_HPP
