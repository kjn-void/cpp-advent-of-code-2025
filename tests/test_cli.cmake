if (NOT DEFINED AOC_EXECUTABLE)
    message(FATAL_ERROR "AOC_EXECUTABLE is required")
endif()

set(pathFixture "${CMAKE_CURRENT_BINARY_DIR}/cli-fixture")
file(MAKE_DIRECTORY "${pathFixture}/input")
file(WRITE "${pathFixture}/input/day01.txt" "L50\r\nR100\r\n")
file(WRITE "${pathFixture}/input/day05.txt" "1-3\r\n\r\n2\r\n4\r\n")

execute_process(COMMAND "${AOC_EXECUTABLE}" 1 5
    WORKING_DIRECTORY "${pathFixture}" RESULT_VARIABLE rcProcess OUTPUT_VARIABLE txtOutput)
if (NOT rcProcess EQUAL 0 OR NOT txtOutput STREQUAL "Day 1\n  Part 1: 2\n  Part 2: 2\nDay 5\n  Part 1: 1\n  Part 2: 3\n")
    message(FATAL_ERROR "Valid CLI run failed: ${rcProcess}: ${txtOutput}")
endif()

foreach(usArgument IN ITEMS "1junk" "13" "2")
    execute_process(COMMAND "${AOC_EXECUTABLE}" "${usArgument}"
        WORKING_DIRECTORY "${pathFixture}" RESULT_VARIABLE rcProcess ERROR_VARIABLE errProcess)
    if (rcProcess EQUAL 0 OR errProcess STREQUAL "")
        message(FATAL_ERROR "CLI accepted invalid argument or missing input: ${usArgument}")
    endif()
endforeach()

execute_process(COMMAND "${AOC_EXECUTABLE}" bad 1
    WORKING_DIRECTORY "${pathFixture}" RESULT_VARIABLE rcProcess OUTPUT_VARIABLE txtOutput ERROR_VARIABLE errProcess)
if (rcProcess EQUAL 0 OR NOT txtOutput MATCHES "Day 1")
    message(FATAL_ERROR "CLI did not continue after an invalid day")
endif()

execute_process(COMMAND "${AOC_EXECUTABLE}"
    WORKING_DIRECTORY "${pathFixture}" RESULT_VARIABLE rcProcess ERROR_VARIABLE errProcess)
if (rcProcess EQUAL 0 OR NOT errProcess MATCHES "Usage:")
    message(FATAL_ERROR "CLI did not report usage for missing arguments")
endif()
