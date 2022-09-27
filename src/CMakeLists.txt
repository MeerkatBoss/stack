add_executable(stack main.cpp)

target_link_libraries(stack libstack)

target_include_directories(stack PRIVATE
                        ${CMAKE_CURRENT_SOURCE_DIR})