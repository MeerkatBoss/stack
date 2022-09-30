add_library(libsafestack safe_stack.cpp)

target_link_libraries(libsafestack PRIVATE libstack)

target_include_directories(libsafestack PUBLIC
                        ${CMAKE_CURRENT_LIST_DIR})