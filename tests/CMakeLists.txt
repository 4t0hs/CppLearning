set(LIB_LINK_DIRECTORY ${CMAKE_BINARY_DIR}/lib/)

set(TEST_OPTIONS
	-Wall -Wextra -O0 -ggdb -g3
)
set(TESTPLUS_OPTIONS
	-fsanitize=address --coverage
)
set(TESTING_FRAMEWORK
	GTest::gtest_main
)

add_subdirectory(SampleTest)
add_subdirectory(AsyncQueueTest)
add_subdirectory(TimerTest)
add_subdirectory(EventFlagTest)
