project(vtapi.demo1)

add_library(vtmodule_demo1 SHARED
    demo1.cpp
)

set_target_properties(vtmodule_demo1 PROPERTIES
    VERSION ${VTAPI_VERSION}
    SOVERSION ${VTAPI_SOVERSION}
    INSTALL_RPATH "${VTAPI_LIBRARY_PATH};${VTAPI_CURRENT_PATH};/usr/lib;/usr/local/lib"
    INSTALL_RPATH_USE_LINK_PATH TRUE
    BUILD_WITH_INSTALL_RPATH TRUE
)

target_link_libraries(vtmodule_demo1
    vtapi
    ${OPENCV_LIBS}
)

target_include_directories(vtmodule_demo1 PRIVATE
    $<TARGET_PROPERTY:vtapi,INTERFACE_INCLUDE_DIRECTORIES>
)

install(TARGETS vtmodule_demo1
    DESTINATION ${VTAPI_MODULES_DIR}
)
