cmake_minimum_required(VERSION 2.8)
project(vtapi.vtapi)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DVTAPI_DEBUG")

file(GLOB ${PROJECT_NAME}.Sources *.cpp)
file(GLOB_RECURSE ${PROJECT_NAME}.Headers ../../include/*.h)

set(VTAPI_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib CACHE FILEPATH "Install path of VTAPI library")
set(VTAPI_CURRENT_PATH ${CMAKE_CURRENT_LIST_DIR} CACHE FILEPATH "Built path of VTAPI library")

add_library(vtapi SHARED
  ${${PROJECT_NAME}.Sources}
  ${${PROJECT_NAME}.Headers}
)

set_target_properties(vtapi PROPERTIES
  VERSION ${VTAPI_VERSION}
  SOVERSION ${VTAPI_SOVERSION}
)
if (OPENCV_LIBDIR)
  set_target_properties(vtapi PROPERTIES
    INSTALL_RPATH ${OPENCV_LIBDIR}
)
endif()

target_link_libraries(vtapi
  ${POCOFOUNDATION_LIBS}
  ${POCOUTIL_LIBS}
  ${OPENCV_LIBS}
)

target_include_directories(vtapi BEFORE PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
  ${OPENCV_INCLUDE_PATH}
)
target_include_directories(vtapi BEFORE PRIVATE
  ${POCOFOUNDATION_INCLUDE_PATH}
  ${POCOUTIL_INCLUDE_PATH}
)


install(TARGETS vtapi
  LIBRARY DESTINATION lib
)

install(DIRECTORY ../../include/
  DESTINATION include
)
