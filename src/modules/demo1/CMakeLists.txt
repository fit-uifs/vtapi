
add_library(vtmodule_demo1 SHARED demo1.cpp)

target_link_libraries(vtmodule_demo1
    vtapi
)

target_include_directories(vtmodule_demo1 PRIVATE
    $<TARGET_PROPERTY:vtapi,INTERFACE_INCLUDE_DIRECTORIES>
)

install(TARGETS vtmodule_demo1
    DESTINATION ${VTAPI_MODULES_DIR}
)
