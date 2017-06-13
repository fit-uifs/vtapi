project(vtapi.demo)

add_library(vtmodule_demo SHARED
    demo.cpp
)

set_target_properties(vtmodule_demo PROPERTIES
    VERSION ${VTAPI_VERSION}
    SOVERSION ${VTAPI_SOVERSION}
    INSTALL_RPATH "${VTAPI_LIBRARY_PATH};${VTAPI_CURRENT_PATH};/usr/lib;/usr/local/lib"
    INSTALL_RPATH_USE_LINK_PATH TRUE
    BUILD_WITH_INSTALL_RPATH TRUE
)

target_link_libraries(vtmodule_demo
    vtapi
    ${OPENCV_LIBS}
)

target_include_directories(vtmodule_demo PRIVATE
    $<TARGET_PROPERTY:vtapi,INTERFACE_INCLUDE_DIRECTORIES>
)

install(TARGETS vtmodule_demo
    DESTINATION ${VTAPI_MODULES_DIR}
)
