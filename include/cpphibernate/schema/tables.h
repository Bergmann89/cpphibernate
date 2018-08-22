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

    }

    /* meta */

    template<typename T>
    struct is_tables
        : public __impl::is_tables_impl<T>
        { };

    /* operations */

    namespace __impl
    {

        /* tables_make_impl */

        template <typename X, typename = void>
        struct tables_make_impl
        {
            template <typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for hibernate::schema::tables::make(...)!"); }
        };

        template<typename... T>
        struct tables_make_impl<mp::list<T...>, mp::enable_if<all_are_tables<T...>>>
        {
            template <typename ...T_tables>
            static constexpr decltype(auto) apply(T_tables&&... tables)
                { return tables_t<T_tables...>(std::forward<T_tables>(tables)...); }
        };

        /* tables_find_impl */

        template<typename X, typename = void>        
        struct tables_find_impl
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for hibernate::schema::tables::find(...)!"); }
        };

        template<typename T_tables, typename T_wrapped_dataset>
        struct tables_find_impl<
            mp::list<T_tables, T_wrapped_dataset>,
            mp::enable_if_c<
               is_tables<mp::decay_t<T_tables>>::value>>
        {
            template<size_t I, size_t N, typename = void>
            struct helper;

            template<size_t N>
            struct helper<N, N, void>
                { static_assert(N != N, "Table for given datatype does not exist!"); };

            template<size_t I, size_t N>
            struct helper<I, N, mp::enable_if_c<
                decltype(hana::not_equal(std::declval<T_tables>()[hana::size_c<I>].wrapped_dataset, T_wrapped_dataset { }))::value>>
            {
                static constexpr decltype(auto) apply(T_tables&& tables)
                    { return helper<I+1, N>::apply(std::forward<T_tables>(tables)); }
            };

            template<size_t I, size_t N>
            struct helper<I, N, mp::enable_if_c<
                decltype(hana::equal(std::declval<T_tables>()[hana::size_c<I>].wrapped_dataset, T_wrapped_dataset { }))::value>>
            {
                static constexpr decltype(auto) apply(T_tables&& tables)
                    { return std::forward<T_tables>(tables)[hana::size_c<I>]; }
            };

            static constexpr decltype(auto) apply(T_tables&& tables, T_wrapped_dataset&&)
            {
                using count_type = mp::decay_t<decltype(hana::size(std::declval<T_tables>()))>;
                return helper<0, count_type::value>::apply(std::forward<T_tables>(tables));
            }
        };

    }

    namespace tables
    {

        constexpr decltype(auto) make = misc::make_generic_predicate<__impl::tables_make_impl> { };

        constexpr decltype(auto) find = misc::make_generic_predicate<__impl::tables_find_impl> { };

    }

}
end_namespace_cpphibernate_schema