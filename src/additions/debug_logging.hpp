// This source file was added to the original project to extend or customize functionality.
// It is licensed under the same terms as the original project.
//
// For license details, see the LICENSE.md or COPYING.txt file in the root of the repository.
//
// Author: @Si1ver, 2025.

#pragma once

#ifndef ADDITIONS_DEBUG_LOGGING_HPP
#define ADDITIONS_DEBUG_LOGGING_HPP

#include <string>

#include "additions/windows.hpp"

namespace additions {

class DebugLogging {
public:
  static void Log(const wchar_t* message) {
    if (IsDebuggerPresent()) {
      OutputDebugStringW(message);
      OutputDebugStringW(L"\n");
    }
  }

  static void Log(const char* message) {
    if (IsDebuggerPresent()) {
      OutputDebugStringA(message);
      OutputDebugStringW(L"\n");
    }
  }

  static void Log(const std::wstring& message) {
    Log(message.c_str());
  }

  static void Log(const std::string& message) {
    Log(message.c_str());
  }
};

}  // namespace additions

#endif  // ADDITIONS_DEBUG_LOGGING_HPP
