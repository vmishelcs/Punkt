add_executable(scanner_test scanner_test.cc)
target_link_libraries(scanner_test scanner GTest::gtest glog::glog)
gtest_discover_tests(scanner_test)
