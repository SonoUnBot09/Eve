add_executable(eve_test
    main.cpp
    S_Entity.cpp
    S_Test.cpp
)

set_target_properties(eve_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_link_libraries(eve_test
    PRIVATE
        Eve
)