cmake_minimum_required(VERSION 2.8)
project(vtapi)

# global C++11 flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# VTApi version
set(VTAPI_VERSION 3.0)
set(VTAPI_SOVERSION 3)

# Additional cmake parameters

# DEFAULT SETTINGS
# default build type is "Release"
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Default build type is \"Release\"")
endif ()
# where to install executables, libraries and headers (relative path to ./build or absolute)
# default install prefix is "/usr/local"
if (NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX /usr/local CACHE FILEPATH "Default install prefix is \"/usr/local\"")
endif()
# where to install module libraries (relative path to INSTALL_PREFIX or absolute)
if (NOT VTAPI_MODULES_DIR)
  set(VTAPI_MODULES_DIR lib CACHE FILEPATH "Default module libraries path is \"${CMAKE_INSTALL_PREFIX}/lib\"")
endif ()

option(BUILD_DOC "Generate doxygen documentation (Doxygen is required)" OFF)

set(DEFAULT_INCLUDE_PATH /usr/include /usr/local/include /include)
set(DEFAULT_LIBRARY_PATH /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu /usr/local/lib/x86_64-linux-gnu /lib64 /lib)


#pkg-config
set(PKG_CONFIG "pkg-config" CACHE FILEPATH "Path of pkg-config executable")
execute_process(COMMAND ${PKG_CONFIG} RESULT_VARIABLE PKG_CONFIG_ERR OUTPUT_QUIET ERROR_QUIET)
if (NOT (PKG_CONFIG_ERR EQUAL 1))
  message(FATAL_ERROR "Executable \"pkg_config\" could not be found. Please, install it.")
endif ()


# POCO libraries
if (POCO_PATH)
  set(POCO_INCLUDE_PATH ${POCO_PATH}/include ${POCO_INCLUDE_PATH} CACHE FILEPATH "Poco headers path")
  set(POCO_LIBRARY_PATH ${POCO_PATH}/lib ${POCO_LIBRARY_PATH} CACHE FILEPATH "Poco libraries path")
endif ()
if (POCO_INCLUDE_PATH)
  set(POCOFOUNDATION_INCLUDE_PATH ${POCOFOUNDATION_INCLUDE_PATH} ${POCO_INCLUDE_PATH} CACHE FILEPATH "Poco Foundation headers path")
  set(POCOUTIL_INCLUDE_PATH ${POCOUTIL_INCLUDE_PATH} ${POCO_INCLUDE_PATH} CACHE FILEPATH "Poco Util headers path")
  set(POCOJSON_INCLUDE_PATH ${POCOJSON_INCLUDE_PATH} ${POCO_INCLUDE_PATH} CACHE FILEPATH "Poco JSON headers path")
  set(POCOXML_INCLUDE_PATH ${POCOXML_INCLUDE_PATH} ${POCO_INCLUDE_PATH} CACHE FILEPATH "Poco XML headers path")
endif ()
if (POCO_LIBRARY_PATH)
  set(POCOFOUNDATION_LIBRARY_PATH ${POCOFOUNDATION_LIBRARY_PATH} ${POCO_LIBRARY_PATH} CACHE FILEPATH "Poco Foundation libraries path")
  set(POCOUTIL_LIBRARY_PATH ${POCOUTIL_LIBRARY_PATH} ${POCO_LIBRARY_PATH} CACHE FILEPATH "Poco Util library path")
  set(POCOJSON_LIBRARY_PATH ${POCOJSON_LIBRARY_PATH} ${POCO_LIBRARY_PATH} CACHE FILEPATH "Poco JSON library path")
  set(POCOXML_LIBRARY_PATH ${POCOXML_LIBRARY_PATH} ${POCO_LIBRARY_PATH} CACHE FILEPATH "Poco XML library path")
endif ()

find_file(TEST_POCOFOUNDATION_INCLUDE_PATH Poco/Foundation.h ${POCO_INCLUDE_PATH} ${POCOFOUNDATION_INCLUDE_PATH} ${DEFAULT_INCLUDE_PATH})
find_file(TEST_POCOUTIL_INCLUDE_PATH Poco/Util/Util.h ${POCO_INCLUDE_PATH} ${POCOUTIL_INCLUDE_PATH} ${DEFAULT_INCLUDE_PATH})
find_file(TEST_POCOJSON_INCLUDE_PATH Poco/JSON/JSON.h ${POCO_INCLUDE_PATH} ${POCOJSON_INCLUDE_PATH} ${DEFAULT_INCLUDE_PATH})
find_file(TEST_POCOXML_INCLUDE_PATH Poco/XML/XML.h ${POCO_INCLUDE_PATH} ${POCOXML_INCLUDE_PATH} ${DEFAULT_INCLUDE_PATH})

find_library(POCOFOUNDATION_LIBS PocoFoundation ${POCO_LIBRARY_PATH} ${POCOFOUNDATION_LIBRARY_PATH} ${DEFAULT_LIBRARY_PATH})
find_library(POCOUTIL_LIBS PocoUtil ${POCO_LIBRARY_PATH} ${POCOUTIL_LIBRARY_PATH} ${DEFAULT_LIBRARY_PATH})
find_library(POCOJSON_LIBS PocoJSON ${POCO_LIBRARY_PATH} ${POCOJSON_LIBRARY_PATH} ${DEFAULT_LIBRARY_PATH})
find_library(POCOXML_LIBS PocoXML ${POCO_LIBRARY_PATH} ${POCOXML_LIBRARY_PATH} ${DEFAULT_LIBRARY_PATH})

if (NOT TEST_POCOFOUNDATION_INCLUDE_PATH)
  message(SEND_ERROR "Poco Foundation include path error. Please, use one of -DPOCO_PATH=<root path to Poco>, -DPOCO_INCLUDE_PATH=<path> or -DPOCOFOUNDATION_INCLUDE_PATH=<path> variable.")
endif ()
if (NOT POCOFOUNDATION_LIBS)
  message(SEND_ERROR "Poco Foundation library was not found. Please, use one of -DPOCO_PATH=<root path to Poco>, -DPOCO_LIBRARY_PATH=<path> or -DPOCOFOUNDATION_LIBRARY_PATH=<path> variable.")
endif ()

if (NOT TEST_POCOUTIL_INCLUDE_PATH)
  message(SEND_ERROR "Poco Util include path error. Please, use one of -DPOCO_PATH=<root path to Poco>, -DPOCO_INCLUDE_PATH=<path> or -DPOCOUTIL_INCLUDE_PATH=<path> variable.")
endif ()
if (NOT POCOUTIL_LIBS)
  message(SEND_ERROR "Poco Util library was not found. Please, use one of -DPOCO_PATH=<root path to Poco>, -DPOCO_LIBRARY_PATH=<path> or -DPOCOUTIL_LIBRARY_PATH=<path> variable.")
endif ()

if (NOT TEST_POCOJSON_INCLUDE_PATH)
  message(SEND_ERROR "Poco JSON include path error. Please, use one of -DPOCO_PATH=<root path to Poco>, -DPOCO_INCLUDE_PATH=<path> or -DPOCOJSON_INCLUDE_PATH=<path> variable.")
endif ()
if (NOT POCOJSON_LIBS)
  message(SEND_ERROR "Poco JSON library was not found. Please, use one of -DPOCO_PATH=<root path to Poco>, -DPOCO_LIBRARY_PATH=<path> or -DPOCOJSON_LIBRARY_PATH=<path> variable.")
endif ()

if (NOT TEST_POCOXML_INCLUDE_PATH)
  message(SEND_ERROR "Poco XML include path error. Please, use one of -DPOCO_PATH=<root path to Poco>, -DPOCO_INCLUDE_PATH=<path> or -DPOCOXML_INCLUDE_PATH=<path> variable.")
endif ()
if (NOT POCOXML_LIBS)
  message(SEND_ERROR "Poco XML library was not found. Please, use one of -DPOCO_PATH=<root path to Poco>, -DPOCO_LIBRARY_PATH=<path> or -DPOCOXML_LIBRARY_PATH=<path> variable.")
endif ()


# OpenCV - required version at least 2.4.2
# if (OPENCV_PATH)
#   find_package(OpenCV 2.4.2 REQUIRED PATHS ${OPENCV_PATH})
# else ()
#   find_package(OpenCV 2.4.2 REQUIRED)
# endif ()
if (OPENCV_PC_PATH)
  if(EXISTS ${OPENCV_PC_PATH})
    execute_process(COMMAND ${PKG_CONFIG} ${OPENCV_PC_PATH} --modversion
      OUTPUT_VARIABLE OPENCV_VERSION # TODO: minimal version check
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(COMMAND ${PKG_CONFIG} ${OPENCV_PC_PATH} --variable=includedir_new
      OUTPUT_VARIABLE OPENCV_INCLUDE_PATH
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(COMMAND ${PKG_CONFIG} ${OPENCV_PC_PATH} --libs
      OUTPUT_VARIABLE OPENCV_LIBS
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(COMMAND ${PKG_CONFIG} ${OPENCV_PC_PATH} --variable=libdir
      OUTPUT_VARIABLE OPENCV_LIBDIR
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    string(REPLACE " " ";" OPENCV_LIBS ${OPENCV_LIBS})
  else ()
    message(SEND_ERROR "OpenCV file for pkg-config could not be found at location \"${OPENCV_PC_PATH}\". Please, check the path and try it again.")
  endif ()
else ()
  # TODO Remake to find_package(OpenCV REQUIRED) ONLY + version checks
  find_package(OpenCV REQUIRED PATHS ${OPENCV_PATH})
  if (OpenCV_VERSION VERSION_LESS 2.4.2)
    message(SEND_ERROR "OpenCV version >= 2.4.2 is required.")
  endif ()
  if (OpenCV_FOUND)
    set(OPENCV_INCLUDE_PATH ${OpenCV_INCLUDE_DIRS})
    set(OPENCV_LIBS ${OpenCV_LIBS})
  endif ()
endif()

if (BUILD_DOC)
  # Doxygen support
  message("-- Documentation configuration")
  find_program(DOXYGEN doxygen
    HINTS ${DOXYGEN_BIN}
  )
  if (DOXYGEN)
    set(PROJECT_NAME_DOC "VTApi")

    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc-config/Doxyfile)
    set(VTAPI_DOC_MAIN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/MAINPAGE.txt.in)
    set(VTAPI_DOC_MAIN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc-config/MAINPAGE.txt)
    set(VTAPI_DOC_SAMPLES_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/SAMPLES.txt.in)
    set(VTAPI_DOC_SAMPLES_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc-config/SAMPLES.txt)

    # requests to configure the files
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    configure_file(${VTAPI_DOC_MAIN_IN} ${VTAPI_DOC_MAIN_OUT} @ONLY)
    configure_file(${VTAPI_DOC_SAMPLES_IN} ${VTAPI_DOC_SAMPLES_OUT} @ONLY)

    set(VTAPI_DOC_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/doc)
    add_custom_target(vtapi_doc ALL
      COMMAND ${CMAKE_COMMAND} -E make_directory ${VTAPI_DOC_OUTDIR}
      COMMAND ${CMAKE_COMMAND} -E chdir ${VTAPI_DOC_OUTDIR} ${DOXYGEN} ${DOXYGEN_OUT}
      COMMENT "Generating VTApi + VTServer documentation with Doxygen"
      VERBATIM
    )
  else (DOXYGEN)
    message(SEND_ERROR "Doxygen executable could not be found. Please do one of following steps, instal doxygen package, specify path by -DDOXYGEN_BIN=<executable doxygen> or run cmake without -DBUILD_DOC parameter.")
  endif()
  message("-- Documentation configuration - done")
endif()


# all sources
add_subdirectory(src)

