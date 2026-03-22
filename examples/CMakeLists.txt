add_executable(basic_list basic_list.cpp)
add_executable(basic_tree basic_tree.cpp)

target_link_libraries(basic_list PRIVATE vis)
target_link_libraries(basic_tree PRIVATE vis)