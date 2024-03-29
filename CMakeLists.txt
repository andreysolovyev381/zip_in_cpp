cmake_minimum_required(VERSION 3.21)
set (PROJECT_NAME zip_in_cpp)
project(${PROJECT_NAME}
        VERSION 1.0
        DESCRIPTION "Zip in Cpp"
        LANGUAGES CXX)

#set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")

if (APPLE)
    #debug
    add_definitions(-D__GLIBCXX__)

    #Threads not found
    if(NOT Threads_FOUND)
        set(CMAKE_THREAD_LIBS_INIT "-lpthread")
        set(CMAKE_HAVE_THREADS_LIBRARY 1)
        set(CMAKE_USE_WIN32_THREADS_INIT 0)
        set(CMAKE_USE_PTHREADS_INIT 1)
        set(THREADS_PREFER_PTHREAD_FLAG ON)
    endif()
endif (APPLE)

find_package(GTest REQUIRED)

if(GTest_FOUND)
    message(STATUS " <===> FOUND GTest, including and linking...")
    include_directories(${GTEST_INCLUDE_DIRS})
    add_executable(${PROJECT_NAME}_tests tests_itertools.cpp)
    target_compile_options(${PROJECT_NAME}_tests
            PRIVATE
            -fsanitize=address -g -fno-omit-frame-pointer -fno-optimize-sibling-calls
            -fsanitize=undefined -g -fno-omit-frame-pointer
            #            -fsanitize=leak -g
            #            -fsanitize=thread -fPIE -pie -g
            )
    target_link_options(${PROJECT_NAME}_tests
            PRIVATE
            -fsanitize=address
            -fsanitize=undefined
            #            -fsanitize=leak
            #            -fsanitize=thread
            )
    target_link_libraries(${PROJECT_NAME}_tests
            GTest::GTest
            pthread
            )

    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME}_tests)

else()
    message(STATUS " <===> NOT FOUND GTest, nothing to run the tests on")
endif()
