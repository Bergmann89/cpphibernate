#pragma once

#include <memory>
#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>

beg_namespace_cpphibernate_driver_mariadb
{
    
    /* schema_t */

    struct schema_t;

    /* make_schema */

    namespace __impl
    {

        template<typename T, typename = void>
        struct make_schema_impl;

    }

    constexpr decltype(auto) make_schema = misc::make_generic_predicate<__impl::make_schema_impl> { };

}
end_namespace_cpphibernate_driver_mariadb