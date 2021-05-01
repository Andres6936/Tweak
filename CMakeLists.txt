CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
PROJECT(Tweat LANGUAGES C)

ADD_LIBRARY(Tweat.Core
        Source/actions.c
        Source/btree.c
        Source/buffer.c
        Source/curses.c
        Source/keytab.c
        Source/rcfile.c
        Source/search.c
        Source/slang.c
        )
TARGET_INCLUDE_DIRECTORIES(Tweat.Core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include/>)

ADD_EXECUTABLE(Tweat.App
        Source/main.c
        )

TARGET_LINK_LIBRARIES(Tweat.App PRIVATE Tweat.Core ncurses)
