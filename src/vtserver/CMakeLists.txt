project (vtserver)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -DBOOST_DATE_TIME_NO_LIB")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DVTSERVER_DEBUG")

file(GLOB ${PROJECT_NAME}.Sources *.cpp *.cc *.h)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}.Sources})

set_target_properties(${PROJECT_NAME} PROPERTIES
  INSTALL_RPATH ${VTAPI_LIBRARY_PATH}
)

find_library(rpcz_LIBRARY rpcz)
find_library(zmq3_LIBRARY zmq zmq3)
find_library(protobuf_LIBRARY protobuf)
find_library(boost_system_LIBRARY boost_system)
find_library(boost_thread_LIBRARY boost_thread)
find_library(boost_program_options_LIBRARY boost_program_options)
find_library(vtapi ${VTAPI_CURRENT_PATH})
target_link_libraries(${PROJECT_NAME}
    vtapi
    ${rpcz_LIBRARY}
    ${zmq3_LIBRARY}
    ${protobuf_LIBRARY}
    ${boost_system_LIBRARY}
    ${boost_thread_LIBRARY}
    ${boost_program_options_LIBRARY}
    ${POCOFOUNDATION_LIBS}
    -pthread
)

target_include_directories(${PROJECT_NAME} BEFORE PRIVATE
    $<TARGET_PROPERTY:vtapi,INTERFACE_INCLUDE_DIRECTORIES>
    ${POCOFOUNDATION_INCLUDE_PATH}
    ${DEFAULT_INCLUDE_PATH}
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
