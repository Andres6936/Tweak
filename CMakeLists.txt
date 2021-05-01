CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
PROJECT(Tweak LANGUAGES C CXX)

ADD_LIBRARY(Tweak.Core
        Source/actions.c
        Source/btree.c
        Source/buffer.c
        Source/curses.c
        Source/keytab.c
        Source/rcfile.c
        Source/search.c
        Source/slang.c
        )
TARGET_INCLUDE_DIRECTORIES(Tweak.Core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include/>)

# Reference: https://cmake.org/cmake/help/latest/module/FindCurses.html
# Set CURSES_NEED_NCURSES to TRUE before the find_package(Curses)
# call if NCurses functionality is required.
SET(CURSES_NEED_NCURSES TRUE)
FIND_PACKAGE(Curses REQUIRED)

IF(CURSES_FOUND)

    ADD_EXECUTABLE(Tweak.App Source/main.c)

    TARGET_INCLUDE_DIRECTORIES(Tweak.App PRIVATE ${CURSES_INCLUDE_DIRS})
    TARGET_LINK_LIBRARIES(Tweak.App PRIVATE Tweak.Core ${CURSES_LIBRARIES})

ELSE()

    MESSAGE(FATAL "Cannot found ncurses in the system")

ENDIF()

