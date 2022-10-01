add_library(liblogs logger.cpp)

target_include_directories(liblogs PUBLIC
                        ${CMAKE_CURRENT_LIST_DIR})