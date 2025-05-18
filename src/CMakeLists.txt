find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)

add_executable(pngav WIN32)

target_sources(pngav PRIVATE
    main.cpp
    registry.cpp
    spi.cpp
    registry.hpp
    spi.hpp
    pngav.rc
    resource.h
    pngav.ico
)

target_link_libraries(pngav PRIVATE ZLIB::ZLIB)
target_link_libraries(pngav PRIVATE PNG::PNG)
target_link_libraries(pngav PRIVATE shlwapi.lib)
