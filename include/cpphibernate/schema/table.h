#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/misc/wrap.h>
#include <cpphibernate/misc/general.h>
#include <cpphibernate/schema/fields.h>
#include <cpphibernate/schema/attribute.h>

beg_namespace_cpphibernate_schema
{

    namespace __impl
    {

        /* table_t */

        template<typename T_name, typename T_wrapped_dataset, typename T_table_id, typename T_fields>
        struct table_t
        {
            using name_type             = T_name;
            using wrapped_dataset_type  = mp::decay_t<T_wrapped_dataset>;
            using dataset_type          = misc::decay_unwrap_t<wrapped_dataset_type>;
            using table_id_type         = T_table_id;
            using fields_type           = T_fields;
            using this_type             = table_t<name_type, wrapped_dataset_type, table_id_type, fields_type>;

            name_type               name;
            wrapped_dataset_type    wrapped_dataset;
            table_id_type           table_id;
            fields_type             fields;

            constexpr table_t(
                T_name              p_name,
                T_wrapped_dataset   p_wrapped_dataset,
                T_table_id          p_table_id,
                T_fields            p_fields)
                : name              (std::forward<T_name>           (p_name))
                , wrapped_dataset   (std::forward<T_wrapped_dataset>(p_wrapped_dataset))
                , table_id          (std::forward<T_table_id>       (p_table_id))
                , fields            (std::forward<T_fields>         (p_fields))
                { }

            cpphibernate_copyable(table_t, delete);
            cpphibernate_moveable(table_t, default);

            cpphibernate_equality_comparable();

            inline void print(std::ostream& os) const
            {
                using namespace ::utl;
                size_t index = 0;
                os  << indent << '{'
                    << incindent
                        << indent << "\"name\": \"" << name << "\","
                        << indent << "\"wrapped_dataset\": \"" << utl::type_helper<misc::decay_unwrap_t<wrapped_dataset_type>>::name() << "\""
                        << indent << "\"table_id\": \"" << hana::value(table_id) << "\""
                        << indent << "\"fields\": "
                        << indent << '['
                        << incindent;
                        hana::for_each(fields, [&](auto& field){
                            if (index++ > 0) os << ",";
                            field.print(os);
                        });
                os      << decindent
                        << indent << ']'
                    << decindent
                    << indent << '}';
            }
        };

    }

    /* meta */

    template<typename T>
    struct is_table
        : public mp::is_specialization_of<T, __impl::table_t>
        { };

    template<typename... T>
    struct all_are_tables
        : mp::all_true<is_table<T>::value...>
        { };

    /* operations */

    namespace __impl
    {

        /* table_make_impl */

        template <typename X, typename = void>
        struct table_make_impl
        {
            template <typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for hibernate::schema::table::make(...)!"); }
        };

        template<typename T_name, typename T_wrapped_dataset, typename T_id, typename T_fields>
        struct table_make_impl<mp::list<T_name, T_wrapped_dataset, T_id, T_fields>, mp::enable_if_c<
                hana::is_a<hana::type_tag, T_wrapped_dataset>
            &&  is_fields<mp::decay_t<T_fields>>::value>>
        {
            static constexpr decltype(auto) apply(T_name&& name, T_wrapped_dataset&& wrapped_dataset, T_id&& id, T_fields&& fields)
            {
                return table_t<T_name, T_wrapped_dataset, T_id, T_fields>(
                    std::forward<T_name>            (name),
                    std::forward<T_wrapped_dataset> (wrapped_dataset),
                    std::forward<T_id>              (id),
                    std::forward<T_fields>          (fields));
            }
        };

        /* table_get_wrapped_dataset_impl */

        struct table_get_wrapped_dataset_impl
        {
            template<typename T_table>
            constexpr decltype(auto) operator()(T_table&& table) const
                { return table.wrapped_dataset; }
        };

        /* table_get_primary_key_field_impl */

        template<typename X, typename = void>
        struct table_get_primary_key_field_impl
        {
            template <typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for hibernate::schema::table::get_primary_key_field(...)!"); }
        };

        template<typename T_table>
        struct table_get_primary_key_field_impl<
            mp::list<T_table>,
            mp::enable_if_c<
                is_table<mp::decay_t<T_table>>::value>>
        {
            template<size_t I>
            using is_primary_key_field = decltype(hana::contains(
                std::declval<T_table>().fields[hana::size_c<I>].attributes,
                attribute::primary_key));

            template<size_t I, size_t N, typename = void>
            struct helper;

            template<size_t N>
            struct helper<N, N, void>
                { static_assert(N != N, "Unable to find primary key field for table!"); };

            template<size_t I, size_t N>
            struct helper<I, N, mp::enable_if_c<
                !is_primary_key_field<I>::value>>
            {
                static constexpr decltype(auto) apply(T_table&& table)
                    { return helper<I+1, N>::apply(std::forward<T_table>(table)); }
            };

            template<size_t I, size_t N>
            struct helper<I, N, mp::enable_if_c<
                is_primary_key_field<I>::value>>
            {
                static constexpr decltype(auto) apply(T_table&& table)
                    { return std::forward<T_table>(table).fields[hana::size_c<I>]; }
            };

            static constexpr decltype(auto) apply(T_table&& table)
            {
                using count = mp::decay_t<decltype(hana::size(std::declval<T_table>().fields))>;
                return helper<0, count::value>::apply(
                    std::forward<T_table>(table));
            }
        };

    }

    namespace table
    {

        constexpr decltype(auto) make = misc::make_generic_predicate<__impl::table_make_impl> { };

        constexpr decltype(auto) get_wrapped_dataset = __impl::table_get_wrapped_dataset_impl { };

        constexpr decltype(auto) get_primary_key_field = misc::make_generic_predicate<__impl::table_get_primary_key_field_impl> { };

    }

}
end_namespace_cpphibernate_schema