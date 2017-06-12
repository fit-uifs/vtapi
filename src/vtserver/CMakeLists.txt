cmake_minimum_required (VERSION 2.8)
project (vtapi.vtserver)


set(DEFAULT_LIBRARY_PATH /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu /usr/local/lib/x86_64-linux-gnu /lib64 /lib)
find_library(rpcz_LIBRARY rpcz ${DEFAULT_LIBRARY_PATH})
find_library(zmq3_LIBRARY zmq zmq3 ${DEFAULT_LIBRARY_PATH})
find_library(protobuf_LIBRARY protobuf ${DEFAULT_LIBRARY_PATH})
find_library(boost_system_LIBRARY boost_system ${DEFAULT_LIBRARY_PATH})
find_library(boost_thread_LIBRARY boost_thread ${DEFAULT_LIBRARY_PATH})
find_library(boost_program_options_LIBRARY boost_program_options ${DEFAULT_LIBRARY_PATH})

# global g++ flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DVTSERVER_DEBUG")



file(GLOB ${PROJECT_NAME}.Sources *.cpp *.cc *.h)

add_executable(vtserver
    ${${PROJECT_NAME}.Sources})

if (OPENCV_LIBDIR)
  set_target_properties(vtserver PROPERTIES
    INSTALL_RPATH ${OPENCV_LIBDIR}
)
endif()

target_link_libraries(vtserver
    vtapi
    ${rpcz_LIBRARY}
    ${zmq3_LIBRARY}
    ${protobuf_LIBRARY}
    ${boost_system_LIBRARY}
    ${boost_thread_LIBRARY}
    ${boost_program_options_LIBRARY}
    ${POCOFOUNDATION_LIBS}
    ${OPENCV_LIBS}
    -pthread
)

target_include_directories(vtserver PUBLIC
    $<TARGET_PROPERTY:vtapi,INTERFACE_INCLUDE_DIRECTORIES>
    ${OPENCV_INCLUDE_PATH}
)

install(TARGETS vtserver
    RUNTIME DESTINATION bin
)

