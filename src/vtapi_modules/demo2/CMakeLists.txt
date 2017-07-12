project(vtmodule_demo2)

file(GLOB ${PROJECT_NAME}.Sources *.cpp *.h)

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}.Sources})

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${VTAPI_VERSION}
    SOVERSION ${VTAPI_SOVERSION}
    INSTALL_RPATH ${VTAPI_LIBRARY_PATH}
)
if (OPENCV_LIBDIR)
  set_target_properties(vtapi PROPERTIES
    INSTALL_RPATH ${OPENCV_LIBDIR}
)
endif()

find_library(vtapi ${VTAPI_CURRENT_PATH})
target_link_libraries(${PROJECT_NAME}
    vtapi
    ${OPENCV_LIBS}
)

target_include_directories(${PROJECT_NAME} BEFORE PRIVATE
    $<TARGET_PROPERTY:vtapi,INTERFACE_INCLUDE_DIRECTORIES>
    ${OPENCV_INCLUDE_PATH}
    ${DEFAULT_INCLUDE_PATH}
)

install(TARGETS ${PROJECT_NAME}
    DESTINATION ${VTAPI_MODULES_DIR}
)
