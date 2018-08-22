#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/misc/general.h>
#include <cpphibernate/schema/field.h>

beg_namespace_cpphibernate_schema
{

    namespace __impl
    {

        /* fields_t */

        template<typename... T_fields>
        using fields_t = hana::tuple<T_fields...>;

        /* is_fields_impl */

        template<typename T, typename = void>
        struct is_fields_impl
            : public mp::c_false_t
            { };

        template<typename... T>
        struct is_fields_impl<fields_t<T...>, mp::enable_if<all_are_fields<T...>>>
            : public mp::c_true_t
            { };

    }

    /* meta */

    template<typename T>
    struct is_fields :
        public __impl::is_fields_impl<T>
        { };

    /* operations */

    namespace __impl
    {

        /* fields_make_impl */

        template <typename T, typename = void>
        struct fields_make_impl
        {
            template <typename ...T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for hibernate::schema::fields::make(...)!"); }
        };

        template<typename... T>
        struct fields_make_impl<mp::list<T...>, mp::enable_if<all_are_fields<T...>>>
        {
            template <typename ...T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { return fields_t<T_args...>(std::forward<T_args>(args)...); }
        };

    }

    namespace fields
    {

        constexpr decltype(auto) make = misc::make_generic_predicate<__impl::fields_make_impl> { };

    }

}
end_namespace_cpphibernate_schema