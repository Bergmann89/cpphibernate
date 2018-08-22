#pragma once

#include <cpphibernate/config.h>

beg_namespace_cpphibernate_driver_mariadb
{
    
    /* attribute_t */

    enum class attribute_t
    {
        hex,
        compress,
        primary_key,
    };

    /* attributes_t */

    struct attributes_t;

    /* make_attributes */

    namespace __impl
    {

        template<typename T, typename = void>
        struct make_attributes_impl;

    }

    constexpr decltype(auto) make_attributes = misc::make_generic_predicate<__impl::make_attributes_impl> { };

}
end_namespace_cpphibernate_driver_mariadb