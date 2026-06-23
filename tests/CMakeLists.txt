find_package(Qt6 REQUIRED COMPONENTS Test)

qt_add_executable(test_ui test_ui.cpp)
target_link_libraries(test_ui PRIVATE simplefoc_lib Qt6::Test)
add_test(NAME test_ui COMMAND test_ui)
