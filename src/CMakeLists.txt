cmake_minimum_required(VERSION 2.8)

project(vtapi.vtapi)

file(GLOB ${PROJECT_NAME}.Sources *.cpp)
file(GLOB_RECURSE ${PROJECT_NAME}.Headers ../include/*.h)

set(VTAPI_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib CACHE FILEPATH "Install path of VTAPI library")
set(VTAPI_CURRENT_PATH ${CMAKE_CURRENT_LIST_DIR} CACHE FILEPATH "Built path of VTAPI library")

add_library(vtapi SHARED
  ${${PROJECT_NAME}.Sources}
  ${${PROJECT_NAME}.Headers}
)

set_target_properties(vtapi PROPERTIES
  VERSION ${VTAPI_VERSION}
  SOVERSION ${VTAPI_SOVERSION}
  INSTALL_RPATH_USE_LINK_PATH TRUE
  BUILD_WITH_INSTALL_RPATH TRUE
)

target_link_libraries(vtapi
  ${POCOFOUNDATION_LIBS}
  ${POCOUTIL_LIBS}
  ${OPENCV_LIBS}
)

target_include_directories(vtapi PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  $<INSTALL_INTERFACE:include>
  ${OPENCV_INCLUDE_PATH}
  ${POCOFOUNDATION_INCLUDE_PATH}
  ${POCOUTIL_INCLUDE_PATH}
  /usr/local/include
)

install(TARGETS vtapi
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
)

install(DIRECTORY ../include/
  DESTINATION include
)
