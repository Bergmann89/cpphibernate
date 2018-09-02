#pragma once

#include <cpphibernate/config.h>

beg_namespace_cpphibernate_modifier
{

    namespace __impl
    {

        /* modifier_t */

        struct modifier_t
            { };

    }

    /* meta */

    template<typename T>
    struct is_modifier
        : public mp::is_base_of<__impl::modifier_t, T>
        { };

    template<typename... T>
    struct all_are_modifiers
        : public mp::all_true<is_modifier<T>::value...>
        { };

}
end_namespace_cpphibernate_modifier