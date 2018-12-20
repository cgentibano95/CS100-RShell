CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

ADD_SUBDIRECTORY(googletest)

SET(CMAKE_CXX_STANDARD 11)

ADD_EXECUTABLE(rshell
		src/main.cpp
		)

	ADD_EXECUTABLE(test
			tests/test.cpp
			)
		TARGET_LINK_LIBRARIES(test gtest gmock_main)
		TARGET_COMPILE_DEFINITIONS(test PRIVATE gtest_disable_pthreads=ON)
