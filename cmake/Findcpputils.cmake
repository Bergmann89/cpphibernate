Include ( ExternalProject )
Include ( FindPackageHandleStandardArgs )

Set ( CPPUTILS_PATH ${CMAKE_BINARY_DIR}/extern/cpputils )

If ( NOT TARGET cpputils_extern )
    ExternalProject_Add ( cpputils_extern
                        PREFIX ${CPPUTILS_PATH}
                        TMP_DIR ${CPPUTILS_PATH}/tmp
                        STAMP_DIR ${CPPUTILS_PATH}/stamp
                        SOURCE_DIR ${CPPUTILS_PATH}/src
                        BINARY_DIR ${CPPUTILS_PATH}/build
                        INSTALL_DIR ${CPPUTILS_PATH}/install
                        GIT_REPOSITORY "https://git.bergmann89.de/cpp/cpputils.git"
                        GIT_TAG "master"
                        TEST_COMMAND make test
                        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
                                   -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
                                   -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                                   -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                                   -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH})
EndIf ( )

Set ( CPPUTILS_LIBRARY ${CPPUTILS_PATH}/install/lib/libcpputils.so )
Set ( CPPUTILS_INCLUDE_DIR ${CPPUTILS_PATH}/install/include )
Set ( CPPUTILS_LIBRARIES ${CPPUTILS_LIBRARY} )
Set ( CPPUTILS_INCLUDE_DIRS ${CPPUTILS_INCLUDE_DIR} )

File ( MAKE_DIRECTORY ${CPPUTILS_INCLUDE_DIR} )

Find_Package_Handle_Standard_Args ( cpputils DEFAULT_MSG
                                    CPPUTILS_LIBRARY
                                    CPPUTILS_INCLUDE_DIR )

If ( NOT TARGET cpputils )
    Add_Library ( cpputils SHARED IMPORTED )
    Add_Dependencies ( cpputils cpputils_extern )
    Set_Property ( TARGET cpputils
                   PROPERTY IMPORTED_LOCATION ${CPPUTILS_LIBRARY} )
    Set_Property ( TARGET cpputils
                   PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CPPUTILS_INCLUDE_DIRS} )
    Install ( FILES ${CPPUTILS_LIBRARY} DESTINATION lib )
    If ( CPPHIBERNATE_INSTALL_DEV_FILES )
        Install ( DIRECTORY ${CPPUTILS_INCLUDE_DIR}/ DESTINATION include )
    EndIf ( )
EndIf ( )