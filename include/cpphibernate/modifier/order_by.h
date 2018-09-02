#pragma once

#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/modifier/modifier.h>

beg_namespace_cpphibernate_modifier
{

    namespace __impl
    {

        /* order_direction_tag_t */

        struct order_direction_tag_t
            { };

        /* order_direction_t */

        template<typename T_field>
        struct order_direction_t
            : public order_direction_tag_t
        {
            using field_type            = T_field;
            using wrapped_field_type    = hana::type<mp::decay_t<field_type>>;

            wrapped_field_type wrapped_field;
        };

    }

    /* meta */

    template<typename T>
    struct is_order_direction
        : public mp::is_base_of<__impl::order_direction_tag_t, T>
        { };

    template<typename... T>
    struct all_are_order_directions
        : public mp::all_true<is_order_direction<T>::value...>
        { };

    namespace __impl
    {

        /* order_ascending_t */

        template<typename T_field>
        struct order_ascending_t
            : public order_direction_t<T_field>
            { };

        /* order_descending_t */

        template<typename T_field>
        struct order_descending_t
            : public order_direction_t<T_field>
            { };

        /* order_by_t */

        template<typename... T_fields>
        struct order_by_t
            : public modifier_t
        {
            using fields_type = hana::basic_tuple<T_fields...>;

            fields_type fields;
        };

    }

    /* meta */

    template<typename T>
    struct is_order_by
        : public mp::is_specialization_of<T, __impl::order_by_t>
        { };

    template<typename T>
    struct is_ascending
        : public mp::is_specialization_of<T, __impl::order_ascending_t>
        { };

    template<typename T>
    struct is_descending
        : public mp::is_specialization_of<T, __impl::order_descending_t>
        { };

    namespace __impl
    {

        /* ascending_builder */

        template<typename X, typename = void>
        struct ascending_builder
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for hibernate::modifier::ascending(...)!"); }
        };

        template<typename T_field>
        struct ascending_builder<
            mp::list<T_field>,
            mp::enable_if<schema::is_field<mp::decay_t<T_field>>>>
        {
            static constexpr decltype(auto) apply(T_field&&)
            {
                using field_type     = mp::decay_t<T_field>;
                using ascending_type = order_ascending_t<field_type>;
                return ascending_type { };
            }
        };

        /* descending_builder */

        template<typename X, typename = void>
        struct descending_builder
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for hibernate::modifier::descending(...)!"); }
        };

        template<typename T_field>
        struct descending_builder<
            mp::list<T_field>,
            mp::enable_if<schema::is_field<mp::decay_t<T_field>>>>
        {
            static constexpr decltype(auto) apply(T_field&&)
            {
                using field_type      = mp::decay_t<T_field>;
                using descending_type = order_descending_t<field_type>;
                return descending_type { };
            }
        };

        /* order_by_builder */

        template<typename X, typename = void>
        struct order_by_builder
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for hibernate::modifier::order_by(...)!"); }
        };

        template<typename... T_order_directions>
        struct order_by_builder<
            mp::list<T_order_directions...>,
            mp::enable_if<all_are_order_directions<mp::decay_t<T_order_directions>...>>>
        {
            static constexpr decltype(auto) apply(T_order_directions&&...)
            {
                using order_by_type = order_by_t<mp::decay_t<T_order_directions>...>;
                return order_by_type { };
            }
        };

    }

    /* make */

    constexpr decltype(auto) ascending  = misc::make_generic_predicate<__impl::ascending_builder>  { };
    constexpr decltype(auto) descending = misc::make_generic_predicate<__impl::descending_builder> { };
    constexpr decltype(auto) order_by   = misc::make_generic_predicate<__impl::order_by_builder>   { };

}
end_namespace_cpphibernate_modifier