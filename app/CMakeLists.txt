add_executable(particle-simulator main.c)
target_link_libraries(particle-simulator engine renderer)

if (EMSCRIPTEN)
    set_target_properties(particle-simulator PROPERTIES OUTPUT_NAME "app")

    target_link_options(particle-simulator PRIVATE
        "SHELL:-s USE_SDL=2"
        "SHELL:-s USE_SDL_TTF=2"
        "SHELL:-s WASM=1"
        "SHELL:-s FULL_ES3=1"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
        "SHELL:-s FORCE_FILESYSTEM=1"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/BitcountSingle-Regular.ttf@BitcountSingle-Regular.ttf"
    )
endif()
