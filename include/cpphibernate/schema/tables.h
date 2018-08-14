#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/misc/general.h>
#include <cpphibernate/schema/table.h>

beg_namespace_cpphibernate_schema
{

    namespace __impl
    {

        /* tables_t */

        template<typename... T_table>
        using tables_t = hana::tuple<T_table...>;

        /* is_tables_impl */

        template<typename T, typename = void>
        struct is_tables_impl
            : mp::c_false_t
            { };

        template<typename... T>
        struct is_tables_impl<tables_t<T...>, mp::enable_if<all_are_tables<T...>>>
            : mp::c_true_t
            { };

        /* tables_builder */

        template <typename X, typename = void>
        struct tables_builder
        {
            template <typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for hibernate::schema::make_tables(...)!"); }
        };

        template<typename... T>
        struct tables_builder<mp::list<T...>, mp::enable_if<all_are_tables<T...>>>
        {
            template <typename ...T_tables>
            static constexpr decltype(auto) apply(T_tables&&... tables)
                { return tables_t<T_tables...>(std::forward<T_tables>(tables)...); }
        };

    }

    /* meta */

    template<typename T>
    struct is_tables
        : public __impl::is_tables_impl<T>
        { };

    /* make */

    constexpr decltype(auto) make_tables = misc::make_generic_predicate<__impl::tables_builder> { };

}
end_namespace_cpphibernate_schema