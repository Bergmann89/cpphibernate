#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/modifier.h>

beg_namespace_cpphibernate_driver_mariadb
{

    namespace __impl
    {

        /* make_modifier_tags_impl */

        template<typename X, typename = void>
        struct make_modifier_tag_impl
        {
            template <typename ...T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for make_modifier_tag(...)!"); }
        };

        template<typename T_modifier>
        struct make_modifier_tag_impl<
            mp::list<T_modifier>,
            mp::enable_if_c<
                    modifier::is_limit_modifier<mp::decay_t<T_modifier>>::value
                ||  modifier::is_offset_modifier<mp::decay_t<T_modifier>>::value>>
        {
            static constexpr decltype(auto) apply(T_modifier&&)
                { return T_modifier { }; }
        };

    }

    constexpr decltype(auto) make_modifier_tag = misc::make_generic_predicate<__impl::make_modifier_tag_impl> { };

    namespace __impl
    {

        /* make_modifier_tags_impl */

        template<typename X, typename = void>
        struct make_modifier_tags_impl
        {
            template <typename ...T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for make_modifier_tags(...)!"); }
        };

        template<typename T_modifiers>
        struct make_modifier_tags_impl<
            mp::list<T_modifiers>,
            mp::enable_if_c<
                modifier::is_modifiers<mp::decay_t<T_modifiers>>::value>>
        {
            static constexpr decltype(auto) apply(T_modifiers&& modifiers)
            {
                return hana::transform(
                    modifiers,
                    make_modifier_tag);
            }
        };

    }

    constexpr decltype(auto) make_modifier_tags = misc::make_generic_predicate<__impl::make_modifier_tags_impl> { };

}
end_namespace_cpphibernate_driver_mariadb
