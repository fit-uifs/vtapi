
# !! this throws error !! all dirs are entered manually below
#find_package(PostgreSQL)


file(GLOB ${PROJECT_NAME}.Sources *.cpp)

add_library(vtapi_postgresql SHARED
    ${${PROJECT_NAME}.Sources})

set_target_properties(vtapi_postgresql PROPERTIES
    VERSION ${VTAPI_VERSION}
    SOVERSION ${VTAPI_SOVERSION}
)

target_link_libraries(vtapi_postgresql
    #${PostgreSQL_LIBRARIES}
    pq
    pqtypes
)

target_include_directories(vtapi_postgresql PRIVATE
    $<TARGET_PROPERTY:vtapi,INTERFACE_INCLUDE_DIRECTORIES>
    #${PostgreSQL_INCLUDE_DIRS}
    /usr/include/postgresql
)

install(TARGETS vtapi_postgresql
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
