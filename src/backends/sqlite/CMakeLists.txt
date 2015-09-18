project(vtapi.sqlite)

if (SQLITE3_PC_PATH)
  if (NOT (EXISTS ${SQLITE3_PC_PATH}))
    message(SEND_ERROR "SQLite3 file for pkg-config could not be found at location \"${SQLITE3_PC_PATH}\". Please, check the path and try it again.")
    unset(SQLITE3_PC_PATH)
  endif ()
else ()
  set(SQLITE3_PC_PATH sqlite3)
  execute_process(COMMAND ${PKG_CONFIG} ${SQLITE3_PC_PATH}
    RESULT_VARIABLE SQLITE3_EXIT_STATUS
    OUTPUT_QUIET
  )
  if (NOT (SQLITE3_EXIT_STATUS EQUAL 0))
    unset(SQLITE3_PC_PATH)
  endif ()
endif()

if(${SQLITE3_PC_PATH})
  execute_process(COMMAND ${PKG_CONFIG} ${SQLITE3_PC_PATH} --modversion
    OUTPUT_VARIABLE SQLITE3_VERSION # TODO: minimal version check
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  execute_process(COMMAND ${PKG_CONFIG} ${SQLITE3_PC_PATH} --variable=includedir
    OUTPUT_VARIABLE SQLITE3_INCLUDE_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  execute_process(COMMAND ${PG_CONFIG_PATH} ${SQLITE3_PC_PATH} --variable=libdir
    OUTPUT_VARIABLE SQLITE3_LIBRARY_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
endif()



file(GLOB ${PROJECT_NAME}.Headers *.h)
file(GLOB ${PROJECT_NAME}.Sources *.cpp)

add_library(vtapi_sqlite SHARED
    ${${PROJECT_NAME}.Headers}
    ${${PROJECT_NAME}.Sources}
)

set_target_properties(vtapi_sqlite PROPERTIES
    VERSION ${VTAPI_VERSION}
    SOVERSION ${VTAPI_SOVERSION}
    INSTALL_RPATH_USE_LINK_PATH TRUE
    BUILD_WITH_INSTALL_RPATH TRUE
)

target_link_libraries(vtapi_sqlite
    sqlite3
)

target_include_directories(vtapi_sqlite PRIVATE
    $<TARGET_PROPERTY:vtapi,INTERFACE_INCLUDE_DIRECTORIES>
)

install(TARGETS vtapi_sqlite
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
