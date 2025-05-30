find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)

find_path(WINREG_INCLUDE_DIRS "winreg/WinReg.hpp")

add_executable(pngav WIN32)

target_sources(pngav PRIVATE
    main.cpp
    spi.cpp
    additions/utilities.cpp
    additions/viewer_window.cpp
    additions/winapi_wrappers.cpp
    additions/susie_plugins/plugin_manager.cpp
    spi.hpp
    additions/debug_logging.hpp
    additions/utilities.hpp
    additions/viewer_window.hpp
    additions/winapi_wrappers.hpp
    additions/windows.hpp
    additions/susie_plugins/plugin_manager.hpp
    resource.rc
    resource.h
    pngav.ico
)

target_include_directories(pngav PRIVATE ${WINREG_INCLUDE_DIRS})
target_include_directories(pngav PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(pngav PRIVATE ZLIB::ZLIB)
target_link_libraries(pngav PRIVATE PNG::PNG)
target_link_libraries(pngav PRIVATE shlwapi.lib)
