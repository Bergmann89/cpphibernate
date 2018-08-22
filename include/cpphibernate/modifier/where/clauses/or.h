#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/modifier/where/clauses/clause.h>

beg_namespace_cpphibernate_modifier
{

    namespace __impl
    {

        /* where_clause_or_t */

        template<typename... T_clauses>
        struct where_clause_or_t
            : public where_clause_t
        {
            using clauses_type = hana::tuple<T_clauses...>;

            clauses_type clauses;

            constexpr where_clause_or_t(T_clauses&&... p_clauses)
                : clauses(std::forward<T_clauses>(p_clauses)...)
                { }
        };

        /* where_clause_or_builder */

        template<typename X, typename = void>
        struct where_clause_or_builder
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for hibernate::modifier::or(...)!"); }
        };

        template<typename... T_clauses>
        struct where_clause_or_builder<
            mp::list<T_clauses...>,
            mp::enable_if<all_are_where_clauses<mp::decay_t<T_clauses>...>>>
        {
            static constexpr decltype(auto) apply(T_clauses&&... clauses)
                { return where_clause_or_t<T_clauses...>(std::forward<T_clauses>(clauses)...); }
        };

    }

    /* meta */

    template<typename T>
    struct is_where_clause_or
        : public mp::is_specialization_of<T, __impl::where_clause_or_t>
        { };

    /* make */

    constexpr decltype(auto) or_ = misc::make_generic_predicate<__impl::where_clause_or_builder> { };

}
end_namespace_cpphibernate_modifier