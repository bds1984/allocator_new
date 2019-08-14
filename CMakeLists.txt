cmake_minimum_required(VERSION 3.2)

project(allocator VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(allocator_cli main.cpp)
add_library(allocator allocator.cpp)
add_executable(my_test my_test.cpp)

set_target_properties(allocator_cli allocator my_test PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(my_test PROPERTIES
    COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
    INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
)

target_link_libraries(allocator_cli PRIVATE
    allocator
)
target_link_libraries(my_test
    ${Boost_LIBRARIES}
    allocator
)

if (MSVC)
    target_compile_options(allocator_cli PRIVATE
        /W4
    )
    target_compile_options(allocator PRIVATE
        /W4
    )
    target_compile_options(my_test PRIVATE
        /W4
    )
else ()
    target_compile_options(allocator_cli PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(allocator PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(my_test PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()



install(TARGETS allocator_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@mail.ru)

include(CPack)

enable_testing()

add_test(my_test my_test)
