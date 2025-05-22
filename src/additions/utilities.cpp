// This source file was added to the original project to extend or customize functionality.
// It is licensed under the same terms as the original project.
//
// For license details, see the LICENSE.md or COPYING.txt file in the root of the repository.
//
// Author: @Si1ver, 2025.

#include "additions/utilities.hpp"

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>

namespace additions {

/**
 * @brief Retrieves the full file path of the current executable.
 *
 * This function uses the Windows API `GetModuleFileNameW` to get the
 * absolute path to the currently running executable file.
 * It handles dynamic buffer resizing in case the path exceeds the initial size.
 *
 * @return std::filesystem::path representing the absolute path of the executable.
 *
 * @throws std::runtime_error if the path cannot be retrieved or if the path
 *         exceeds the maximum allowed buffer size.
 */
std::filesystem::path Utilities::getExecutablePath() {
  constexpr std::size_t initial_buffer_size = 1024;
  constexpr std::size_t no_grow_buffer_size = 32ULL * 1024;

  std::vector<wchar_t> buffer(initial_buffer_size);

  for (;;) {
    constexpr DWORD failed_to_fetch_module_name = 0;
    const DWORD path_length = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));

    if (path_length == failed_to_fetch_module_name) {
      const DWORD error_code = GetLastError();
      throw std::runtime_error(
        "Failed to get executable path. GetModuleFileNameW returned "
        + std::to_string(failed_to_fetch_module_name) + " with error code "
        + std::to_string(error_code));
    }

    if (path_length < buffer.size()) {
      return { buffer.begin(), buffer.begin() + path_length };
    }

    if (path_length == buffer.size()) {
      // Buffer is not big enough to fit the path.
      const DWORD error_code = GetLastError();

      if (error_code == ERROR_INSUFFICIENT_BUFFER) {
        if (buffer.size() > no_grow_buffer_size) {
          // Prevent runaway allocation.
          throw std::runtime_error(
            "Failed to get executable path. Path is larger than maximum buffer size of "
            + std::to_string(buffer.size()));
        }

        buffer.resize(buffer.size() * 2);
        continue;
      }

      // Any other error code is unexpected.
      throw std::runtime_error(
        "Failed to get executable path. GetModuleFileNameW returned buffer size with unexpected error code "
        + std::to_string(error_code));
    }

    // Unexpected result: path_length is greater than buffer.size().
    // This should never happen according to GetModuleFileNameW documentation.
    std::unreachable();
  }
}

}  // namespace additions
