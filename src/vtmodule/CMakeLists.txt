
add_executable(vtmodule vtmodule.cpp)

set_target_properties(vtmodule PROPERTIES
  INSTALL_RPATH "${VTAPI_LIBRARY_PATH};${VTAPI_CURRENT_PATH};/usr/lib;/usr/local/lib"
    INSTALL_RPATH_USE_LINK_PATH TRUE
    BUILD_WITH_INSTALL_RPATH TRUE
)

target_link_libraries(vtmodule
    vtapi
    ${POCOJSON_LIBS}
    ${POCOXML_LIBS}
)

target_include_directories(vtmodule PRIVATE
    $<TARGET_PROPERTY:vtapi,INTERFACE_INCLUDE_DIRECTORIES>
)

install(TARGETS vtmodule
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
