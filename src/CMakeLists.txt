
find_package(OpenCV REQUIRED)

file(GLOB ${PROJECT_NAME}.Sources *.cpp)
file(GLOB_RECURSE ${PROJECT_NAME}.Headers ../include/*.h)

add_library(vtapi SHARED
    ${${PROJECT_NAME}.Sources}
    ${${PROJECT_NAME}.Headers}
)

set_target_properties(vtapi PROPERTIES
    VERSION ${VTAPI_VERSION}
    SOVERSION ${VTAPI_SOVERSION}
)

target_link_libraries(vtapi
    ${PocoFoundation_LIBRARY}
    ${PocoNet_LIBRARY}
    ${PocoUtil_LIBRARY}
    ${OpenCV_LIBRARIES}
)

target_include_directories(vtapi PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
    /usr/local/include
    ${OpenCV_INCLUDE_DIRS}
)

install(TARGETS vtapi
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)

install(DIRECTORY ../include/
    DESTINATION include
)
