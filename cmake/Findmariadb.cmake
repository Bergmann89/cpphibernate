Find_Library ( MARIADB_LIBRARY
               NAMES mariadb mysql
               PATH_SUFFIXES mariadb )

Find_File ( MARIADB_INCLUDE_DIR
            NAMES mysql.h
            PATH_SUFFIXES mariadb )

Get_Filename_Component ( MARIADB_INCLUDE_DIR
                         ${MARIADB_INCLUDE_DIR}
                         DIRECTORY )

Include ( FindPackageHandleStandardArgs )

Find_Package_Handle_Standard_Args ( mariadb  DEFAULT_MSG
                                    MARIADB_LIBRARY
                                    MARIADB_INCLUDE_DIR )

Mark_As_Advanced ( MARIADB_LIBRARY
                   MARIADB_LIBRARIES )

If ( MARIADB_FOUND )
    Set ( MARIADB_LIBRARIES ${MARIADB_LIBRARY} )
    Set ( MARIADB_INCLUDE_DIRS ${MARIADB_INCLUDE_DIR} )

    If ( NOT TARGET mariadb )
        Add_Library ( mariadb UNKNOWN IMPORTED )
        Set_Property ( TARGET mariadb
                       PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${MARIADB_INCLUDE_DIRS}" )
        Set_Property ( TARGET mariadb
                       APPEND
                       PROPERTY IMPORTED_LOCATION "${MARIADB_LIBRARY}")
    EndIf ( )
EndIf ( )
