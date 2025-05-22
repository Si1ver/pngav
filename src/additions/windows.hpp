// This source file was added to the original project to extend or customize functionality.
// It is licensed under the same terms as the original project.
//
// For license details, see the LICENSE.md or COPYING.txt file in the root of the repository.
//
// Author: @Si1ver, 2025.

// Use this header file to include Windows headers instead of directly including Windows.h.

#pragma once

#ifndef ADDITIONS_WINDOWS_HPP
#define ADDITIONS_WINDOWS_HPP

// To properly include Windows headers, some macros need to be defined.

// Documentation links:
// https://learn.microsoft.com/en-gb/windows/win32/winprog/using-the-windows-headers
// https://learn.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-170

// Set minimum supported version to Windows 10.
#ifdef _WIN32_WINNT
#warning "Redefining _WIN32_WINNT to 0x0A00 (Windows 10)."
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT 0x0A00  // _WIN32_WINNT_WIN10

#ifdef WINVER
#warning "Redefining WINVER to 0x0A00 (Windows 10)."
#undef WINVER
#endif

#define WINVER 0x0A00  // _WIN32_WINNT_WIN10

#ifdef NTDDI_VERSION
#warning "Redefining NTDDI_VERSION to 0x0A000000 (Windows 10)."
#undef NTDDI_VERSION
#endif

#define NTDDI_VERSION 0x0A000000  // NTDDI_WIN10

#include <sdkddkver.h>

// Exclude APIs such as Cryptography, DDE, RPC, Shell, and Windows Sockets.
#define WIN32_LEAN_AND_MEAN

// Do not define macros min(a,b) and max(a,b).
#define NOMINMAX

#include <windows.h>

#endif  // ADDITIONS_WINDOWS_HPP
