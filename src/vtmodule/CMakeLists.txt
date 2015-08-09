
add_executable(vtmodule vtmodule.cpp)

target_link_libraries(vtmodule
    vtapi
)

target_include_directories(vtmodule PRIVATE
    $<TARGET_PROPERTY:vtapi,INTERFACE_INCLUDE_DIRECTORIES>
)

install(TARGETS vtmodule
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
