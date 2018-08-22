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

            constexpr offset_t(T_value&& p_value)
                : value(std::forward<T_value>(p_value))
                { }
        };

        /* offset_builder */

        template<typename X, typename = void>
        struct offset_builder
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for hibernate::modifier::offset(...)!"); }
        };

        template<typename T_value>
        struct offset_builder<
            mp::list<T_value>,
            mp::enable_if<mp::is_integral<T_value>>>
        {
            static constexpr decltype(auto) apply(T_value&& value)
            {
                using value_type = mp::integral_constant<T_value, value>;
                return offset_t<value_type>(value_type { });
            }
        };

    }

    /* meta */

    template<typename T>
    struct is_offset_modifier
        : public mp::is_specialization_of<T, __impl::offset_t>
        { };

    /* make */

    constexpr decltype(auto) offset = misc::make_generic_predicate<__impl::offset_builder> { };

}
end_namespace_cpphibernate_modifier