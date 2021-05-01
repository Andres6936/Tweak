CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
PROJECT(Tweak LANGUAGES C)

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

ADD_EXECUTABLE(Tweak.App
        Source/main.c
        )

TARGET_LINK_LIBRARIES(Tweak.App PRIVATE Tweak.Core ncurses)
