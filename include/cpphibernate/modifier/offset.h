#pragma once

#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/modifier/modifier.h>

beg_namespace_cpphibernate_modifier
{

    namespace __impl
    {

        /* offset_t */

        template<typename T_value>
        struct offset_t
            : public modifier_t
        {
            using value_type = T_value;

            value_type value;
        };

    }

    /* meta */

    template<typename T>
    struct is_offset
        : public mp::is_specialization_of<T, __impl::offset_t>
        { };

    /* make */

    template<size_t T_value>
    constexpr decltype(auto) offset = __impl::offset_t<hana::size_t<T_value>> { };

}
end_namespace_cpphibernate_modifier