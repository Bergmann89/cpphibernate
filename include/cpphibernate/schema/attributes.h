#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/misc.h>
#include <cpphibernate/schema/attribute.h>

beg_namespace_cpphibernate_schema
{

    namespace __impl
    {

        /* attributes_t */

        template<typename... T_attributes>
        using attributes_t = hana::tuple<T_attributes...>;

        /* is_attributes_impl */

        template<typename T, typename = void>
        struct is_attributes_impl
            : mp::c_false_t
            { };

        template<typename... T>
        struct is_attributes_impl<attributes_t<T...>, mp::enable_if<all_are_attribures<T...>>>
            : mp::c_true_t
            { };

    }

    /* meta */

    template<typename T>
    struct is_attributes
        : public __impl::is_attributes_impl<T>
        { };

    /* operations */

    namespace __impl
    {

        /* attributes_make_impl */

        template<typename T, typename = void>
        struct attributes_make_impl
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&...)
                { static_assert(sizeof...(T_args) == -1, "Invalid parameters for hibernate::schema::attributes::make(...)!"); }
        };

        template<typename... T>
        struct attributes_make_impl<mp::list<T...>, mp::enable_if_c<
            all_are_attribures<mp::decay_t<T>...>::value>>
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&...)
                { return attributes_t<mp::decay_t<T_args>...> { }; }
        };        

    }

    namespace attributes
    {

        constexpr decltype(auto) make = misc::make_generic_predicate<__impl::attributes_make_impl> { };

    }

}
end_namespace_cpphibernate_schema