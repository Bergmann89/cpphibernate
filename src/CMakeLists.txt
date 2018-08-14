# Initialize ######################################################################################

Include                     ( cotire OPTIONAL )
Include                     ( pedantic OPTIONAL )
Include                     ( strip_symbols OPTIONAL )

Option                      ( BUILD_SHARED_CPPHIBERNATE "Build cpphibernate shared library" OFF )

Set                         ( BUILD_SHARED_LIBS ${BUILD_SHARED_CPPHIBERNATE} )
Set                         ( CMAKE_CXX_STANDARD 17 )
Set                         ( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${PEDANTIC_C_FLAGS}" )
Set                         ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PEDANTIC_CXX_FLAGS}" )

# Project: cpphibernate ###############################################################################

Project                     ( cpphibernate VERSION 1.0.0.0 LANGUAGES CXX )
File                        ( GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
Add_Library                 ( cpphibernate ${SOURCE_FILES} )
Target_Include_Directories  (
                                cpphibernate
                                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
                            )
If                          ( __COTIRE_INCLUDED )
    Cotire                      ( cpphibernate )
EndIf                       ( )
If                          ( __STRIP_SYMBOLS_INCLUDED )
    Strip_Symbols               ( cpphibernate DBG_FILE )
EndIf                       ()
