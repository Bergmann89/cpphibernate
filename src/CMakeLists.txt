# Initialize ######################################################################################

Include                     ( cotire OPTIONAL )
Include                     ( pedantic OPTIONAL )
Include                     ( strip_symbols OPTIONAL )

Include                     ( ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/options.cmake )

Set                         ( BUILD_SHARED_LIBS ${CPPHIBERNATE_BUILD_SHARED} )
Set                         ( CMAKE_CXX_STANDARD 17 )
Set                         ( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${PEDANTIC_C_FLAGS}" )
Set                         ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PEDANTIC_CXX_FLAGS}" )

# Dependencies ####################################################################################

Find_Package                ( cpputils REQUIRED )
Find_Package                ( cppmariadb REQUIRED )

# Project: cpphibernate ###############################################################################

# Build
Project                     ( cpphibernate VERSION 1.0.0.0 LANGUAGES CXX )
Set                         ( CPPHIBERNATE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include )
Set                         ( CPPHIBERNATE_GENERATE_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated )
File                        ( GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
Add_Library                 ( cpphibernate ${SOURCE_FILES} )
Configure_File              ( ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/options.h.in
                              ${CPPHIBERNATE_GENERATE_DIR}/cpphibernate/options.h )
Target_Include_Directories  ( cpphibernate
                              PUBLIC ${CPPHIBERNATE_INCLUDE_DIR}
                                     ${CPPHIBERNATE_GENERATE_DIR} )
Target_Link_Libraries       ( cpphibernate
                              cpputils
                              cppmariadb )

# Install
If                          ( BUILD_SHARED_LIBS OR CPPHIBERNATE_INSTALL_DEV_FILES )
    Install                     ( TARGETS cpphibernate DESTINATION lib )
EndIf                       ( )

If                          ( CPPHIBERNATE_INSTALL_DEV_FILES )
    Install                     ( FILES ${CPPHIBERNATE_INCLUDE_DIR}/cpphibernate.h DESTINATION include )
    Install                     ( DIRECTORY ${CPPHIBERNATE_INCLUDE_DIR}/cpphibernate DESTINATION include )
    Install                     ( DIRECTORY ${CPPHIBERNATE_GENERATE_DIR}/cpphibernate DESTINATION include )
EndIf                       ( )

# Optimize
If                          ( __COTIRE_INCLUDED )
    Cotire                      ( cpphibernate )
EndIf                       ( )
If                          ( __STRIP_SYMBOLS_INCLUDED AND BUILD_SHARED_LIBS )
    Strip_Symbols               ( cpphibernate DBG_FILE )
EndIf                       ()
