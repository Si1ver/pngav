// This source file was added to the original project to extend or customize functionality.
// It is licensed under the same terms as the original project.
//
// For license details, see the LICENSE.md or COPYING.txt file in the root of the repository.
//
// Author: @Si1ver, 2025.

#include "additions/susie_plugins/plugin_manager.hpp"

#include <string>

#include "additions/windows.hpp"

#include <winreg/WinReg.hpp>

#include "additions/utilities.hpp"

namespace additions::susie_plugins {

/**
 * @brief Retrieves the directory path where Susie plug-in files (*.spi) are stored.
 *
 * This function first attempts to read the plug-in path from the Windows Registry:
 *     HKEY_CURRENT_USER\Software\Takechin\Susie\Plug-in\Path
 *
 * If the registry key exists and points to a valid filesystem location, that path is returned.
 * Otherwise, the function falls back to the directory of the current executable.
 *
 * @return std::filesystem::path to the directory containing Susie plug-in files.
 */
std::filesystem::path PluginManager::getPluginPath()
{
  const std::wstring plugin_registry_key_path { LR"(Software\Takechin\Susie\Plug-in)" };

  winreg::RegKey plugin_key;
  winreg::RegResult result = plugin_key.TryOpen(HKEY_CURRENT_USER, plugin_registry_key_path);

  if (result.IsOk()) {
    const auto plugin_path_value = plugin_key.TryGetStringValue(L"Path");

    if (plugin_path_value.IsValid()) {
      const std::wstring &plugin_path_string = plugin_path_value.GetValue();

      if (!plugin_path_string.empty()) {
        try {
          const std::filesystem::path plugin_path = { plugin_path_string };

          if (std::filesystem::is_directory(plugin_path)) {
            return plugin_path;
          }
        } catch (...) {
          // Path from registry is invalid (e.g., malformed or inaccessible).
          // Silently fall back to executable directory.
          // TODO: Consider logging the exception for diagnostics.
        }
      }
    }
  }

  // Fallback: use the directory of the current executable.
  const std::filesystem::path executable_path = Utilities::getExecutablePath();
  return executable_path.parent_path();
}

}  // namespace additions::susie_plugins
