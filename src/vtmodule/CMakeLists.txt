cmake_minimum_required(VERSION 2.8)
project(vtapi.vtmodule)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DVTMODULE_DEBUG")

add_executable(vtmodule vtmodule.cpp)

set_target_properties(vtmodule PROPERTIES
  INSTALL_RPATH ${VTAPI_LIBRARY_PATH}
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
)
