Include ( ExternalProject )
Include ( FindPackageHandleStandardArgs )

Set ( CPPMARIADB_PATH ${CMAKE_BINARY_DIR}/extern/cppmariadb )

If ( NOT TARGET cppmariadb_extern )
    ExternalProject_Add ( cppmariadb_extern
                        PREFIX ${CPPMARIADB_PATH}
                        TMP_DIR ${CPPMARIADB_PATH}/tmp
                        STAMP_DIR ${CPPMARIADB_PATH}/stamp
                        SOURCE_DIR ${CPPMARIADB_PATH}/src
                        BINARY_DIR ${CPPMARIADB_PATH}/build
                        INSTALL_DIR ${CPPMARIADB_PATH}/install
                        GIT_REPOSITORY "https://git.bergmann89.de/cpp/cppmariadb.git"
                        GIT_TAG "master"
                        TEST_COMMAND make test
                        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
                                   -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
                                   -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                                   -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                                   -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH})
EndIf ( )

Set ( CPPMARIADB_LIBRARY ${CPPMARIADB_PATH}/install/lib/libcppmariadb.so )
Set ( CPPMARIADB_INCLUDE_DIR ${CPPMARIADB_PATH}/install/include )
Set ( CPPMARIADB_LIBRARIES ${CPPMARIADB_LIBRARY} )
Set ( CPPMARIADB_INCLUDE_DIRS ${CPPMARIADB_INCLUDE_DIR} )

File ( MAKE_DIRECTORY ${CPPMARIADB_INCLUDE_DIR} )

Find_Package_Handle_Standard_Args ( cppmariadb DEFAULT_MSG
                                    CPPMARIADB_LIBRARY
                                    CPPMARIADB_INCLUDE_DIR )

If ( NOT TARGET cppmariadb )
    Add_Library ( cppmariadb SHARED IMPORTED )
    Add_Dependencies ( cppmariadb cppmariadb_extern )
    Set_Property ( TARGET cppmariadb
                   PROPERTY IMPORTED_LOCATION ${CPPMARIADB_LIBRARY} )
    Set_Property ( TARGET cppmariadb
                   PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CPPMARIADB_INCLUDE_DIRS} )
    Install ( FILES ${CPPMARIADB_LIBRARY} DESTINATION lib )
    If ( CPPHIBERNATE_INSTALL_DEV_FILES )
        Install ( DIRECTORY ${CPPMARIADB_INCLUDE_DIR}/ DESTINATION include )
    EndIf ( )
EndIf ( )