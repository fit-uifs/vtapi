
# !! this throws error !! all dirs are entered manually below
project(vtapi.postgresql)

file(GLOB ${PROJECT_NAME}.Headers *.h)
file(GLOB ${PROJECT_NAME}.Sources *.cpp)

add_library(vtapi_postgresql SHARED
    ${${PROJECT_NAME}.Headers}
    ${${PROJECT_NAME}.Sources}
)

set_target_properties(vtapi_postgresql PROPERTIES
    VERSION ${VTAPI_VERSION}
    SOVERSION ${VTAPI_SOVERSION}
)

target_link_libraries(vtapi_postgresql
    pq
    pqtypes
)

target_include_directories(vtapi_postgresql PRIVATE
    $<TARGET_PROPERTY:vtapi,INTERFACE_INCLUDE_DIRECTORIES>
    /usr/include/postgresql
)

install(TARGETS vtapi_postgresql
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
