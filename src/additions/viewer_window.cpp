// This source file was added to the original project to extend or customize functionality.
// It is licensed under the same terms as the original project.
//
// For license details, see the LICENSE.md or COPYING.txt file in the root of the repository.
//
// Author: @Si1ver, 2025.

#include "additions/viewer_window.hpp"

#include "additions/winapi_wrappers.hpp"

namespace additions {

/**
 * @brief Destructor for the ViewerWindow class.
 * It destroys the viewer window if it exists.
 */
ViewerWindow::~ViewerWindow() {
  if (viewer_window_handle != nullptr) {
    DestroyWindow(viewer_window_handle);
    viewer_window_handle = nullptr;
  }
}

/**
 * @brief Attempts to create a viewer window.
 * @param module_instance The instance handle of the module.
 * @param window_procedure The window procedure function to handle messages for the window.
 */
std::unique_ptr<ViewerWindow> ViewerWindow::TryCreateViewerWindow(HINSTANCE module_instance, WNDPROC window_procedure)
{
  Resources resources = {};

  if (!TryLoadResources(module_instance, resources)) {
    // TODO: Log diagnostic message.
    return nullptr;
  }

  auto window_class = WinApiWrappers::TryRegisterWindowClass(
      module_instance,
      L"PNGAlphaViewer",
      window_procedure,
      0,  // Default window style.
      resources.icon_handle,
      nullptr,
      resources.cursor_handle_hand,
      nullptr,  // Application handles background drawing by itself.
      0,  // No menu.
      0,  // No extra data stored with window class.
      sizeof(ViewerWindow*));  // Pointer to implementation is stored with window instance.

  if (!window_class.has_value()) {
    // TODO: Log diagnostic message.
    return nullptr;
  }

  auto window_handle = WinApiWrappers::TryCreateWindow(
      module_instance,
      window_class.value(),
      nullptr,  // No parent window.
      nullptr,  // No menu.
      WS_EX_ACCEPTFILES | WS_EX_LAYERED,  // Layered window allows transparent drawing.
      WS_OVERLAPPEDWINDOW,
      L"PNG Alpha Viewer",
      CW_USEDEFAULT, CW_USEDEFAULT,
      200, 100);

  if (!window_handle.has_value()) {
    // TODO: Log diagnostic message.
    return nullptr;
  }

  // The `std::make_unique` will not work with ViewerWindow here
  // because the constructor is private and this method is static.
  return std::unique_ptr<ViewerWindow>(new ViewerWindow(module_instance, window_handle.value()));
}

/**
 * @brief Shows the viewer window with the specified show command.
 * @param show_command The command to show the window (e.g., SW_SHOW, SW_HIDE).
 */
void ViewerWindow::Show(int show_command) {
  if (viewer_window_handle == nullptr) {
    // TODO: Log diagnostic message.
    return;
  }

  ShowWindow(viewer_window_handle, show_command);
  UpdateWindow(viewer_window_handle);
}

/**
 * @brief Runs the event loop for the viewer window.
 * @details This function processes messages for the window until a WM_QUIT message is received.
 * @return Returns the exit code from the message loop, or -1 if an error occurred.
 * @note The messages for the window and the thread are processed in this loop.
 */
int ViewerWindow::RunWindowEventLoop() {
  if (viewer_window_handle == nullptr) {
    // TODO: Log diagnostic message.
    return -1;
  }

  MSG message = {};
  int get_message_result;

  while ((get_message_result = GetMessageW(&message, nullptr, 0, 0))) {
    if (get_message_result == -1) {
      // Error occurred while getting message.
      // TODO: Log diagnostic message.
      return -1;
    }

    TranslateMessage(&message);
    DispatchMessageW(&message);
  }

  return static_cast<int>(message.wParam);
}

/**
 * @brief Attempts to load resources required for the viewer window.
 * @param module_instance The instance handle of the module.
 * @param resources The Resources structure to fill with loaded resources.
 * @return Returns true if resources were loaded successfully, false otherwise.
 * @note The resources are loaded as shared resources, so they do not need to be freed manually.
 */
bool ViewerWindow::TryLoadResources(HINSTANCE module_instance, Resources &resources)
{
  // TODO: Use resource ID from resource.h.
  // For now, a hardcoded value is used to avoid including file from parent directory.
  const int icon_resource_id = 101;

  auto icon_handle = WinApiWrappers::TryLoadIconFromModuleResource(module_instance, icon_resource_id);
  if (!icon_handle.has_value()) {
    // TODO: Log diagnostic message.
    return false;
  }

  auto cursor_handle = WinApiWrappers::TryLoadSystemCursor(SystemCursor::Cross);
  if (!cursor_handle.has_value()) {
    // TODO: Log diagnostic message.
    return false;
  }

  resources.icon_handle = icon_handle.value();
  resources.cursor_handle_hand = cursor_handle.value();

  return true;
}

}  // namespace additions
