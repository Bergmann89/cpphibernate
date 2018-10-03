# Initialize ######################################################################################

Include                     ( cotire OPTIONAL )
Include                     ( pedantic OPTIONAL )
Include                     ( cmake_tests OPTIONAL )

Include                     ( ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/options.cmake )

Set                         ( CMAKE_CXX_STANDARD 17 )
Set                         ( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${PEDANTIC_C_FLAGS}" )
Set                         ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PEDANTIC_CXX_FLAGS}" )

# Project: test_cpphibernate ##########################################################################

Project                     ( test_cpphibernate )
File                        ( GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
List                        ( FILTER SOURCE_FILES EXCLUDE REGEX "/_[A-Za-z0-9_-]*\.cpp$" )
Add_Executable              ( test_cpphibernate EXCLUDE_FROM_ALL ${SOURCE_FILES} )
Target_Link_Libraries       ( test_cpphibernate
                              cpphibernate
                              gtest
                              gmock
                              gmock_main
                              pthread )
If                          ( __COTIRE_INCLUDED )
    Cotire                      ( test_cpphibernate )
EndIf                       ( )
If                          ( __CMAKE_TESTS_INCLUDED )
    Add_CMake_Test             ( NAME cpphibernate
                                 TARGET test_cpphibernate )
EndIf                       ( )
