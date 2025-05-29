// This source file was added to the original project to extend or customize functionality.
// It is licensed under the same terms as the original project.
//
// For license details, see the LICENSE.md or COPYING.txt file in the root of the repository.
//
// Author: @Si1ver, 2025.

#pragma once

#ifndef ADDITIONS_WINAPI_WRAPPERS_HPP
#define ADDITIONS_WINAPI_WRAPPERS_HPP

#include <string_view>
#include <optional>

#include "additions/windows.hpp"

namespace additions {

/**
 * @brief Enum for system cursor IDs.
 */
enum struct SystemCursor {
  Arrow = 32512,  // IDC_ARROW
  IBeam = 32513,  // IDC_IBEAM
  Wait = 32514,  // IDC_WAIT
  Cross = 32515,  // IDC_CROSS
  UpArrow = 32516,  // IDC_UPARROW
  SizeNWSE = 32642,  // IDC_SIZENWSE
  SizeNESW = 32643,  // IDC_SIZENESW
  SizeWE = 32644,  // IDC_SIZEWE
  SizeNS = 32645,  // IDC_SIZENS
  SizeAll = 32646,  // IDC_SIZEALL
  No = 32648,  // IDC_NO
  Hand = 32649,  // IDC_HAND
  AppStarting = 32650,  // IDC_APPSTARTING
  Help = 32651,  // IDC_HELP
  Pin = 32671,  // IDC_PIN
  Person = 32672,  // IDC_PERSON
};

/**
 * @brief Class containing various WinAPI wrapper functions.
 * @details This class provides static methods useful for implementing simple window functionality using WinAPI.
 */
class WinApiWrappers {
public:
  static [[nodiscard]] std::optional<HICON> TryLoadIconFromModuleResource(HINSTANCE module_instance, int icon_resource_id);

  static [[nodiscard]] std::optional<HCURSOR> TryLoadSystemCursor(SystemCursor cursor_id);

  static [[nodiscard]] std::optional<ATOM> TryRegisterWindowClass(
    HINSTANCE module_instance,
    std::wstring_view class_name,
    WNDPROC window_procedure,
    UINT window_style,
    HICON icon_handle,
    HICON small_icon_handle,
    HCURSOR cursor_handle,
    HBRUSH background_brush_handle,
    int menu_resource_id,
    int extra_class_bytes,
    int extra_window_bytes);

  static [[nodiscard]] std::optional<HWND> TryCreateWindow(
    HINSTANCE module_instance,
    ATOM window_class_atom,
    HWND parent_window_handle,
    HMENU menu_handle,
    DWORD extended_window_style,
    DWORD window_style,
    std::wstring_view window_title,
    int position_x,
    int position_y,
    int width,
    int height);
};

}  // namespace additions

#endif  // ADDITIONS_WINAPI_WRAPPERS_HPP
