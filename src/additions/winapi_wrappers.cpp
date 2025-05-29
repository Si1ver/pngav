// This source file was added to the original project to extend or customize functionality.
// It is licensed under the same terms as the original project.
//
// For license details, see the LICENSE.md or COPYING.txt file in the root of the repository.
//
// Author: @Si1ver, 2025.

#include "additions/winapi_wrappers.hpp"

namespace additions {

/**
 * @brief Load an icon from the module's resources.
 * @details This function attempts to load an icon from the specified module's resources using the given resource ID.
 * @param module_instance The instance handle of the module from which to load the icon.
 * @param icon_resource_id The resource ID of the icon to load.
 * @return Returns an optional containing the handle to the loaded icon if successful, or std::nullopt if loading failed.
 * @note The loaded icon will be loaded as a shared resource. It does not need to be freed manually.
 */
std::optional<HICON> WinApiWrappers::TryLoadIconFromModuleResource(HINSTANCE module_instance, int icon_resource_id)
{
  HICON icon_handle = static_cast<HICON>(LoadImageW(
    module_instance,
    MAKEINTRESOURCEW(icon_resource_id),
    IMAGE_ICON,
    0, 0,
    LR_DEFAULTSIZE | LR_SHARED));

  return icon_handle != nullptr
    ? std::make_optional(icon_handle)
    : std::nullopt;
}

/**
 * @brief Load a system cursor.
 * @details This function attempts to load a system cursor by its ID.
 * @param cursor_id The ID of the system cursor to load.
 * @return Returns an optional containing the handle to the loaded cursor if successful, or std::nullopt if loading failed.
 * @note The loaded cursor will be loaded as a shared resource. It does not need to be freed manually.
 */
std::optional<HCURSOR> WinApiWrappers::TryLoadSystemCursor(SystemCursor cursor_id)
{
  HCURSOR cursor_handle = static_cast<HCURSOR>(LoadImageW(
    nullptr,
    MAKEINTRESOURCEW(cursor_id),
    IMAGE_CURSOR,
    0, 0,
    LR_DEFAULTSIZE | LR_SHARED));

  return cursor_handle != nullptr
    ? std::make_optional(cursor_handle)
    : std::nullopt;
}

/**
 * @brief Register a window class.
 * @details This function attempts to register a window class with the specified parameters.
 * @param module_instance The instance handle of the module registering the class.
 * @param class_name The name of the window class to register.
 * @param window_procedure The window procedure function.
 * @param window_style The style of the window class.
 * @param icon_handle The handle to the icon.
 * @param small_icon_handle The handle to the small icon.
 * @param cursor_handle The handle to the cursor.
 * @param background_brush_handle The handle to the background brush.
 * @param menu_resource_id The resource ID of the menu associated with the class, or 0 if none.
 * @param extra_class_bytes Extra bytes for class data, or 0 if none.
 * @param extra_window_bytes Extra bytes for window data, or 0 if none.
 * @return Returns an optional containing the atom of the registered window class if successful, or std::nullopt if registration failed.
 */
std::optional<ATOM> WinApiWrappers::TryRegisterWindowClass(
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
  int extra_window_bytes)
{
  WNDCLASSEXW window_class_info = {};

  window_class_info.cbSize = sizeof(window_class_info);

  window_class_info.cbClsExtra = extra_class_bytes;
  window_class_info.cbWndExtra = extra_window_bytes;
  window_class_info.hbrBackground = background_brush_handle;
  window_class_info.hCursor = cursor_handle;
  window_class_info.hIcon = icon_handle;
  window_class_info.hIconSm = small_icon_handle;
  window_class_info.hInstance = module_instance;
  window_class_info.lpfnWndProc = window_procedure;
  window_class_info.lpszClassName = class_name.data();
  window_class_info.lpszMenuName = menu_resource_id != 0
    ? MAKEINTRESOURCEW(menu_resource_id)
    : nullptr;
  window_class_info.style = window_style;

  ATOM window_class_atom = RegisterClassExW(&window_class_info);

  return window_class_atom != 0
    ? std::make_optional(window_class_atom)
    : std::nullopt;
}

/**
 * @brief Create a window.
 * @details This function attempts to create a window with the specified parameters.
 * @param module_instance The instance handle of the module creating the window.
 * @param window_class_atom The atom of the registered window class.
 * @param parent_window_handle The handle to the parent window, or nullptr if none.
 * @param menu_handle The handle to the menu, or nullptr if none.
 * @param extended_window_style The extended style of the window.
 * @param window_style The style of the window.
 * @param window_title The title of the window.
 * @param position_x The x-coordinate of the window's position. Use CW_USEDEFAULT for default positioning.
 * @param position_y The y-coordinate of the window's position. Use CW_USEDEFAULT for default positioning.
 * @param window_width The width of the window.
 * @param window_height The height of the window.
 * @return Returns an optional containing the handle to the created window if successful, or std::nullopt if creation failed.
 */
std::optional<HWND> WinApiWrappers::TryCreateWindow(
  HINSTANCE module_instance,
  ATOM window_class_atom,
  HWND parent_window_handle,
  HMENU menu_handle,
  DWORD extended_window_style,
  DWORD window_style,
  std::wstring_view window_title,
  int position_x,
  int position_y,
  int window_width,
  int window_height)
{
  HWND window_handle = CreateWindowExW(
    extended_window_style,
    (LPCWSTR)MAKEINTATOM(window_class_atom),
    window_title.data(),
    window_style,
    position_x,
    position_y,
    window_width,
    window_height,
    parent_window_handle,
    menu_handle,
    module_instance,
    nullptr);

  return window_handle != nullptr
    ? std::make_optional(window_handle)
    : std::nullopt;
}

}  // namespace additions
