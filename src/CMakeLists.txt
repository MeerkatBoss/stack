add_executable(stack main.cpp)

target_link_libraries(stack libsafestack libutils liblogs)

target_include_directories(stack PRIVATE
                        ${CMAKE_CURRENT_SOURCE_DIR})