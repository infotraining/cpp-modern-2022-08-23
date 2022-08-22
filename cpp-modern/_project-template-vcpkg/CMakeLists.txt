##################
# Target
get_filename_component(DIRECTORY_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" TARGET_MAIN ${DIRECTORY_NAME})

####################
# Sources & headers
aux_source_directory(. SRC_LIST)
file(GLOB HEADERS_LIST "*.h" "*.hpp")

add_executable(${TARGET_MAIN} ${SRC_LIST} ${HEADERS_LIST})

find_package(Catch2 CONFIG REQUIRED)
target_link_libraries(${TARGET_MAIN} PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
target_compile_features(${TARGET_MAIN} PRIVATE cxx_std_20)

include(CTest)
include(Catch)
catch_discover_tests(${TARGET_MAIN})