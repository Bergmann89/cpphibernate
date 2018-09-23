Option                      ( CPPHIBERNATE_BUILD_SHARED
                              "Build cpphibernate shared library"
                              OFF )
Option                      ( CPPHIBERNATE_DEBUG_OUTPUT
                              "Enable debug output"
                              OFF )

If                          ( CPPHIBERNATE_DEBUG_OUTPUT )
    Add_Definitions             ( -DCPPHIBERNATE_DEBUG )
EndIf                       ( )