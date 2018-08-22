#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/schema/field.h>
#include <cpphibernate/modifier/where/clauses/clause.h>

beg_namespace_cpphibernate_modifier
{

    namespace __impl
    {

        /* where_clause_equal_t */

        template<typename T_field, typename T_value>
        struct where_clause_equal_t
            : public where_clause_t
        {
            using field_type = T_field;
            using value_type = T_value;

            field_type field;
            value_type value;

            constexpr where_clause_equal_t(T_field&& p_field, T_value&& p_value)
                : field(std::forward<T_field>(p_field))
                , value(std::forward<T_value>(p_value))
                { }
        };

        /* where_clause_equal_builder */

        template<typename X, typename = void>
        struct where_clause_equal_builder
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for hibernate::modifier::equal(...)!"); }
        };

        template<typename T_field, typename T_value>
        struct where_clause_equal_builder<
            mp::list<T_field, T_value>,
            mp::enable_if<schema::is_field<mp::decay_t<T_field>>>>
        {
            static constexpr decltype(auto) apply(T_field&& field, T_value&& value)
                { return where_clause_equal_t<T_field, T_value>(std::forward<T_field>(field), std::forward<T_value>(value)); }
        };

    }

    /* meta */

    template<typename T>
    struct is_where_clause_equal
        : public mp::is_specialization_of<T, __impl::where_clause_equal_t>
        { };

    /* make */

    constexpr decltype(auto) equal = misc::make_generic_predicate<__impl::where_clause_equal_builder> { };

}
end_namespace_cpphibernate_modifier