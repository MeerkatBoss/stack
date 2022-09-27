add_library(libstack INTERFACE)

target_link_libraries(libstack INTERFACE libutils)

target_include_directories(libstack INTERFACE
                        ${CMAKE_CURRENT_LIST_DIR})