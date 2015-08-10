cmake_minimum_required (VERSION 2.8)
project (vtapi)

# VTApi version
set (VTAPI_VERSION 3.0)
set (VTAPI_SOVERSION 3)

# user-supplied modules dir
set(VTAPI_MODULES_DIR "modules" CACHE STRING "where to install VTApi modules")

# global g++ flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DBOOST_DATE_TIME_NO_LIB")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DVTAPI_DEBUG")

# POCO libraries
find_library(PocoFoundation_LIBRARY PocoFoundation /usr/local/lib)
find_library(PocoNet_LIBRARY PocoNet /usr/local/lib)
find_library(PocoUtil_LIBRARY PocoUtil /usr/local/lib)
find_library(PocoXML_LIBRARY PocoXML /usr/local/lib)


# VTApi library
add_subdirectory(src)

# PostgreSQL backend plugin
add_subdirectory(src/backends/postgresql)

# SQLite backend plugin
add_subdirectory(src/backends/sqlite)

# VTCLi command line tool
add_subdirectory(src/vtcli)

# VTModule module loader
add_subdirectory(src/vtmodule)

# demo1 module
add_subdirectory(src/modules/demo1)

# demo2 module
add_subdirectory(src/modules/demo2)
