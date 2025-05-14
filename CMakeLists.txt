cmake_minimum_required(VERSION 3.11)

project(SDL2_image)

# Zmienna na nazwę binarki
set(EXECUTABLE_NAME ferrets_grape)

add_executable(${EXECUTABLE_NAME}
    main.cpp
    sprite/sprite.cpp
    player/player.cpp
    block/block.cpp
    map/map.cpp
    level_manager/level_manager.cpp
    camera/camera.cpp
)

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)

target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
)

if(PSP)
    # Create an EBOOT.PBP file
    create_pbp_file(
        TARGET ${EXECUTABLE_NAME}
        ICON_PATH NULL
        BACKGROUND_PATH NULL
        PREVIEW_PATH NULL
        TITLE "Ferrets grape"
        VERSION 01.00
    )
endif()

