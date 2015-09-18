project(vtapi.vtcli)

add_executable(vtcli
    vtcli.cpp
)

set_target_properties(vtcli PROPERTIES
  INSTALL_RPATH "${VTAPI_LIBRARY_PATH};${VTAPI_CURRENT_PATH};/usr/lib;/usr/local/lib"
    INSTALL_RPATH_USE_LINK_PATH TRUE
    BUILD_WITH_INSTALL_RPATH TRUE
)

target_link_libraries(vtcli
    vtapi
    ${POCOXML_LIBS}
    ${OPENCV_LIBS}
)

target_include_directories(vtcli PRIVATE
    $<TARGET_PROPERTY:vtapi,INTERFACE_INCLUDE_DIRECTORIES>
)

install(TARGETS vtcli
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
