project(vtapi.vtcli)

add_executable(vtcli
    vtcli.cpp
)

target_link_libraries(vtcli
    vtapi
    ${PocoXML_LIBRARY}
)

target_include_directories(vtcli PRIVATE
    $<TARGET_PROPERTY:vtapi,INTERFACE_INCLUDE_DIRECTORIES>
)

install(TARGETS vtcli
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
