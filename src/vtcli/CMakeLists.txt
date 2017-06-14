project(vtapi.vtcli)

file(GLOB ${PROJECT_NAME}.Sources *.cpp *.h)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}.Sources})

set_target_properties(${PROJECT_NAME} PROPERTIES
    INSTALL_RPATH ${VTAPI_LIBRARY_PATH}
)

find_library(vtapi ${VTAPI_CURRENT_PATH})
target_link_libraries(${PROJECT_NAME}
    vtapi
)

target_include_directories(${PROJECT_NAME} BEFORE PRIVATE
    $<TARGET_PROPERTY:vtapi,INTERFACE_INCLUDE_DIRECTORIES>
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
