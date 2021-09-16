cmake_minimum_required(VERSION 3.12.4)

project(hepek_engine)

# entt
add_subdirectory(external/entt)

# bx
add_library(bx STATIC
    external/bx/src/amalgamated.cpp
)
target_include_directories(bx
PUBLIC
    external/bx/include
PRIVATE
    external/bx/3rdparty
)

# astc (image decoding)
set(OPTION_ASTC_TESTS OFF)
add_subdirectory(external/bimg/3rdparty/astc-codec)

# bimg
add_library(bimg STATIC
    external/bimg/src/image_cubemap_filter.cpp
    external/bimg/src/image_decode.cpp
    external/bimg/src/image_encode.cpp
    external/bimg/src/image_gnf.cpp
    external/bimg/src/image.cpp
)
target_include_directories(bimg
PUBLIC
    external/bimg/include
PRIVATE
    external/bimg/3rdparty
    external/bimg/3rdparty/iqa/include
    external/bimg/3rdparty/tinyexr/deps/miniz
)
target_link_libraries(bimg bx astc-codec)

# bgfx
add_library(bgfx STATIC
    external/bgfx/src/amalgamated.cpp
    external/bgfx/3rdparty
)
target_include_directories(bgfx
PUBLIC
    external/bgfx/include
PRIVATE
    external/bgfx/3rdparty
    external/bgfx/3rdparty/khronos
)
target_link_libraries(bgfx bx bimg X11 GL)

# sdl2
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)
add_subdirectory(external/SDL)

# imgui
add_library(imgui STATIC
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_sdl.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)
target_link_libraries(imgui SDL2-static)
target_include_directories(imgui PUBLIC external/imgui)

add_executable(test-compile test-compile.cpp)
target_link_libraries(test-compile EnTT bgfx bx bimg imgui SDL2-static)
