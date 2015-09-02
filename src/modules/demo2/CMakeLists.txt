project(vtapi.demo2)

add_library(vtmodule_demo2 SHARED
    demo2.cpp
)

target_link_libraries(vtmodule_demo2
    vtapi
    ${OpenCV_LIBRARIES}
)

target_include_directories(vtmodule_demo2 PRIVATE
    $<TARGET_PROPERTY:vtapi,INTERFACE_INCLUDE_DIRECTORIES>
)

install(TARGETS vtmodule_demo2
    DESTINATION ${VTAPI_MODULES_DIR}
)
