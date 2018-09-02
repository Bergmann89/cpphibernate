#pragma once

#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/modifier/modifier.h>

beg_namespace_cpphibernate_modifier
{

    namespace __impl
    {

        /* is_modifiers_impl */

        template<typename T, typename = void>
        struct is_modifiers_impl
            : mp::c_false_t
            { };

        template<typename... T>
        struct is_modifiers_impl<hana::basic_tuple<T...>, mp::enable_if<all_are_modifiers<T...>>>
            : mp::c_true_t
            { };

    }

    /* meta */

    template<typename T>
    struct is_modifiers
        : public __impl::is_modifiers_impl<T>
        { };

    /* operations */

    namespace __impl
    {

        /* make_modifiers_impl */

        template<typename T, typename = void>
        struct make_modifiers_impl
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&...)
                { static_assert(sizeof...(T_args) == -1, "Invalid parameters for hibernate::schema::modifier::make(...)!"); }
        };

        template<typename... T>
        struct make_modifiers_impl<
            mp::list<T...>,
            mp::enable_if_c<
                all_are_modifiers<mp::decay_t<T>...>::value>>
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { return hana::make_basic_tuple(std::forward<T_args>(args)...); }
        };

    }

    constexpr decltype(auto) make_list = misc::make_generic_predicate<__impl::make_modifiers_impl> { };

}
end_namespace_cpphibernate_modifier