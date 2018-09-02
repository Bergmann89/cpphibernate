#pragma once

#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/modifier/modifier.h>

beg_namespace_cpphibernate_modifier
{

    namespace __impl
    {

        /* limit_t */

        template<typename T_value>
        struct limit_t
            : public modifier_t
        {
            using value_type = T_value;

            value_type value;
        };

    }

    /* meta */

    template<typename T>
    struct is_limit_modifier
        : public mp::is_specialization_of<T, __impl::limit_t>
        { };

    /* make */

    template<size_t T_value>
    constexpr decltype(auto) limit = __impl::limit_t<hana::size_t<T_value>> { };

}
end_namespace_cpphibernate_modifier