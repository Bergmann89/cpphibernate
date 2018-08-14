#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/misc/wrap.h>
#include <cpphibernate/misc/general.h>
#include <cpphibernate/schema/fields.h>

beg_namespace_cpphibernate_schema
{

    namespace __impl
    {

        /* table_t */

        template<typename T_name, typename T_dataset_wrapped, typename T_table_id, typename T_fields>
        struct table_t
        {
            using name_type             = T_name;
            using dataset_wrapped_type  = mp::decay_t<T_dataset_wrapped>;
            using table_id_type         = T_table_id;
            using fields_type           = T_fields;
            using this_type             = table_t<name_type, dataset_wrapped_type, table_id_type, fields_type>;

            name_type               name;
            dataset_wrapped_type    dataset_wrapped;
            table_id_type           table_id;
            fields_type             fields;

            constexpr table_t(
                T_name              p_name,
                T_dataset_wrapped   p_dataset_wrapped,
                T_table_id          p_table_id,
                T_fields            p_fields)
                : name              (std::forward<T_name>           (p_name))
                , dataset_wrapped   (std::forward<T_dataset_wrapped>(p_dataset_wrapped))
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
                        << indent << "\"dataset_wrapped\": \"" << utl::type_helper<misc::decay_unwrap_t<dataset_wrapped_type>>::name() << "\""
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

        /* table_builder */

        template <typename X, typename = void>
        struct table_builder
        {
            template <typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for hibernate::schema::make_table(...)!"); }
        };

        template<typename T_name, typename T_dataset_wrapped, typename T_id, typename T_fields>
        struct table_builder<mp::list<T_name, T_dataset_wrapped, T_id, T_fields>, mp::enable_if_c<
                hana::is_a<hana::type_tag, T_dataset_wrapped>
            &&  is_fields<mp::decay_t<T_fields>>::value>>
        {
            static constexpr decltype(auto) apply(T_name&& name, T_dataset_wrapped&& dataset_wrapped, T_id&& id, T_fields&& fields)
            {
                return table_t<T_name, T_dataset_wrapped, T_id, T_fields>(
                    std::forward<T_name>            (name),
                    std::forward<T_dataset_wrapped> (dataset_wrapped),
                    std::forward<T_id>              (id),
                    std::forward<T_fields>          (fields));
            }
        };

    }

    /* meta */

    template<typename T>
    struct is_table : mp::is_specialization_of<T, __impl::table_t> { };

    template<typename... T>
    struct all_are_tables : mp::all_true<is_table<T>::value...> { };

    /* make */

    constexpr decltype(auto) make_table = misc::make_generic_predicate<__impl::table_builder> { };

}
end_namespace_cpphibernate_schema