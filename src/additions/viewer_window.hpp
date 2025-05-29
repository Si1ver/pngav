// This source file was added to the original project to extend or customize functionality.
// It is licensed under the same terms as the original project.
//
// For license details, see the LICENSE.md or COPYING.txt file in the root of the repository.
//
// Author: @Si1ver, 2025.

#pragma once

#ifndef ADDITIONS_VIEWER_WINDOW_HPP
#define ADDITIONS_VIEWER_WINDOW_HPP

#include <memory>

#include "additions/windows.hpp"

namespace additions {

// This class is an intermediate step in the decomposition of the viewer window.
// It should be decomposed further into project-independent simple window class and project-specific viewer window class.
class ViewerWindow {
public:
  ~ViewerWindow();

  static [[nodiscard]] std::unique_ptr<ViewerWindow> TryCreateViewerWindow(HINSTANCE module_instance, WNDPROC window_procedure);

  void Show(int show_command);

  int RunWindowEventLoop();

private:
  struct Resources {
    HICON icon_handle;
    HCURSOR cursor_handle_hand;
  };

  HINSTANCE module_instance;
  HWND viewer_window_handle;

  /**
   * @brief Constructor for the ViewerWindow class.
   * @details This constructor initializes the ViewerWindow with the given module instance and window handle.
   * @param module_instance The instance handle of the module creating the window.
   * @param window_handle The handle to the viewer window.
   * @note The constructor is private to ensure that instances can only be created through the static TryCreateViewerWindow method.
   */
  ViewerWindow(HINSTANCE module_instance, HWND &window_handle)
   : module_instance(module_instance), viewer_window_handle(window_handle) {};

  static [[nodiscard]] bool TryLoadResources(HINSTANCE module_instance, Resources &resources);
};

}  // namespace additions

#endif  // ADDITIONS_VIEWER_WINDOW_HPP
