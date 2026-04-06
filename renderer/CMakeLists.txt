set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/imgui)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

add_library(renderer
    renderer.c
    gui.cpp
    ${IMGUI_SOURCES}
)

target_include_directories(renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(renderer PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

if (EMSCRIPTEN)
    # SDL2 and SDL2_ttf headers live in the Emscripten sysroot.
    # Pass USE_SDL/USE_SDL_TTF at compile time so the port headers are resolved.
    target_compile_options(renderer PRIVATE
        "SHELL:-s USE_SDL=2"
        "SHELL:-s USE_SDL_TTF=2"
    )
    target_link_libraries(renderer engine m)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

    target_include_directories(renderer PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
    )
    target_link_libraries(renderer engine ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES} m)
endif()
