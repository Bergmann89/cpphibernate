#pragma once

#include <vector>

#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/schema/field.fwd.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* fields_t */

    struct fields_t;

    /* make_fields */

    namespace __impl
    {

        template<typename T, typename = void>
        struct make_fields_impl;

    }

    constexpr decltype(auto) make_fields = misc::make_generic_predicate<__impl::make_fields_impl> { };

}
end_namespace_cpphibernate_driver_mariadb