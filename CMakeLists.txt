cmake_minimum_required(VERSION 3.16)
project(C-Scroller)

set(CMAKE_C_STANDARD 11)

include(FetchContent)

FetchContent_Declare(
    inih
    URL https://github.com/benhoyt/inih/archive/refs/tags/r62.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(inih)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

# Add include directories
include_directories(include)

set(SOURCES
    src/main.c
    src/renderer.c
    src/text_entity.c
    src/config.c
    ${inih_SOURCE_DIR}/ini.c
)

add_executable(banner ${SOURCES})

target_include_directories(banner PRIVATE 
    ${SDL2_INCLUDE_DIRS} 
    ${SDL2_TTF_INCLUDE_DIRS}
    ${inih_SOURCE_DIR}
)
target_link_libraries(banner ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})
target_compile_options(banner PRIVATE ${SDL2_CFLAGS_OTHER} ${SDL2_TTF_CFLAGS_OTHER})