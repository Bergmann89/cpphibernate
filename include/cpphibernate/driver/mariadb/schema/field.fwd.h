#pragma once

#include <memory>
#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* field_t */

    struct field_t;

    /* field_ptr_t */

    using field_ptr_t = std::unique_ptr<field_t>;

    /* make_field */

    namespace __impl
    {

        template<typename T, typename = void>
        struct make_field_impl;

    }

    constexpr decltype(auto) make_field = misc::make_generic_predicate<__impl::make_field_impl> { };

}
end_namespace_cpphibernate_driver_mariadb