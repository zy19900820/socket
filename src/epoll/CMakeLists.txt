FILE(GLOB SRC           *.cpp *.h)

SOURCE_GROUP(""         FILES ${SRC})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

add_executable(epoll ${SRC})

TARGET_LINK_LIBRARIES(epoll pthread)

