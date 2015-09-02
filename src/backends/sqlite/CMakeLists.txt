project(vtapi.sqlite)

file(GLOB ${PROJECT_NAME}.Headers *.h)
file(GLOB ${PROJECT_NAME}.Sources *.cpp)

add_library(vtapi_sqlite SHARED
    ${${PROJECT_NAME}.Headers}
    ${${PROJECT_NAME}.Sources}
)

set_target_properties(vtapi_sqlite PROPERTIES
    VERSION ${VTAPI_VERSION}
    SOVERSION ${VTAPI_SOVERSION}
)

target_link_libraries(vtapi_sqlite
    sqlite3
)

target_include_directories(vtapi_sqlite PRIVATE
    $<TARGET_PROPERTY:vtapi,INTERFACE_INCLUDE_DIRECTORIES>
)

install(TARGETS vtapi_sqlite
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
