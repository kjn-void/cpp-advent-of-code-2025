if (NOT DEFINED AOC_EXECUTABLE)
    message(FATAL_ERROR "AOC_EXECUTABLE is required")
endif()

set(fixture "${CMAKE_CURRENT_BINARY_DIR}/cli-fixture")
file(MAKE_DIRECTORY "${fixture}/input")
file(WRITE "${fixture}/input/day01.txt" "L50\r\nR100\r\n")
file(WRITE "${fixture}/input/day05.txt" "1-3\r\n\r\n2\r\n4\r\n")

execute_process(COMMAND "${AOC_EXECUTABLE}" 1 5
    WORKING_DIRECTORY "${fixture}" RESULT_VARIABLE status OUTPUT_VARIABLE output)
if (NOT status EQUAL 0 OR NOT output STREQUAL "Day 1\n  Part 1: 2\n  Part 2: 2\nDay 5\n  Part 1: 1\n  Part 2: 3\n")
    message(FATAL_ERROR "Valid CLI run failed: ${status}: ${output}")
endif()

foreach(argument IN ITEMS "1junk" "13" "2")
    execute_process(COMMAND "${AOC_EXECUTABLE}" "${argument}"
        WORKING_DIRECTORY "${fixture}" RESULT_VARIABLE status ERROR_VARIABLE error)
    if (status EQUAL 0 OR error STREQUAL "")
        message(FATAL_ERROR "CLI accepted invalid argument or missing input: ${argument}")
    endif()
endforeach()

execute_process(COMMAND "${AOC_EXECUTABLE}" bad 1
    WORKING_DIRECTORY "${fixture}" RESULT_VARIABLE status OUTPUT_VARIABLE output ERROR_VARIABLE error)
if (status EQUAL 0 OR NOT output MATCHES "Day 1")
    message(FATAL_ERROR "CLI did not continue after an invalid day")
endif()

execute_process(COMMAND "${AOC_EXECUTABLE}"
    WORKING_DIRECTORY "${fixture}" RESULT_VARIABLE status ERROR_VARIABLE error)
if (status EQUAL 0 OR NOT error MATCHES "Usage:")
    message(FATAL_ERROR "CLI did not report usage for missing arguments")
endif()
