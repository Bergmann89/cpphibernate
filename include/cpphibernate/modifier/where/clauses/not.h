#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/modifier/where/clauses/clause.h>

beg_namespace_cpphibernate_modifier
{

    namespace __impl
    {

        /* where_clause_not_t */

        template<typename T_clause>
        struct where_clause_not_t
            : public where_clause_t
        {
            using clause_type = T_clause;

            clause_type clause;

            constexpr where_clause_not_t(T_clause&& p_clause)
                : clause(std::forward<T_clause>(p_clause))
                { }
        };

        /* where_clause_not_builder */

        template<typename X, typename = void>
        struct where_clause_not_builder
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for hibernate::modifier::not(...)!"); }
        };

        template<typename T_clause>
        struct where_clause_not_builder<
            mp::list<T_clause>,
            mp::enable_if<is_where_clause<mp::decay_t<T_clause>>>>
        {
            static constexpr decltype(auto) apply(T_clause&& clause)
                { return where_clause_not_t<T_clause>(std::forward<T_clause>(clause)); }
        };

    }

    /* meta */

    template<typename T>
    struct is_where_clause_not
        : public mp::is_specialization_of<T, __impl::where_clause_not_t>
        { };

    /* make */

    constexpr decltype(auto) not_ = misc::make_generic_predicate<__impl::where_clause_not_builder> { };

}
end_namespace_cpphibernate_modifier