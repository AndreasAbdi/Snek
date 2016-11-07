#--------------------------------------------------
# FILE ORGANIZATION
#--------------------------------------------------
file (GLOB_RECURSE SRC_GAME
    "${CMAKE_CURRENT_SOURCE_DIR}/game/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/game/*.h"
)

source_group("game" FILES ${SRC_GAME})

#--------------------------------------------------
# EXECUTABLE BUILDING
#--------------------------------------------------
set (SNEK_SOURCES
    Main.cpp
    ${SRC_GAME}
)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
ADD_EXECUTABLE ( Snek ${SNEK_SOURCES} )

#--------------------------------------------------
# SDL
#--------------------------------------------------

set (SDL_INCLUDE_DIR "C:/programming/libraries/sdl_2_04/include")
set (SDL_LIBRARY "C:/programming/libraries/sdl_2_04/lib/x86/SDL2.lib")
set (SDL_MAIN_LIBRARY "C:/programming/libraries/sdl_2_04/lib/x86/SDL2main.lib")

INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR})
TARGET_LINK_LIBRARIES (Snek ${SDL_LIBRARY} ${SDL_MAIN_LIBRARY})

# SDL - ttf
set (SDL_TTF_LIBRARY "C:/programming/libraries/sdl_2_ttf/lib/x86/SDL2_ttf.lib")
INCLUDE_DIRECTORIES("C:/programming/libraries/sdl_2_ttf/include")
TARGET_LINK_LIBRARIES(Snek ${SDL_TTF_LIBRARY})

#--------------------------------------------------
# RESOURCES
#--------------------------------------------------
# Add the resource files to the executable's directory.

#this is for assets
add_custom_command(TARGET Snek POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets"
        $<TARGET_FILE_DIR:Snek>/assets)

#this is for dlls
add_custom_command(TARGET Snek POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/libs"
        $<TARGET_FILE_DIR:Snek>)
