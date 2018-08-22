#pragma once

#include <vector>

#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/schema/field.fwd.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* tables_t */

    struct tables_t;

    /* make_tables */

    namespace __impl
    {

        template<typename T, typename = void>
        struct make_tables_impl;

    }

    constexpr decltype(auto) make_tables = misc::make_generic_predicate<__impl::make_tables_impl> { };

}
end_namespace_cpphibernate_driver_mariadb