#pragma once

#include <cpphibernate/driver/mariadb/schema/fields.h>

beg_namespace_cpphibernate_driver_mariadb
{

    namespace __impl
    {

        /* make_fields_impl */

        template<typename T, typename>
        struct make_fields_impl
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for mariadb::make_fields(...)!"); }
        };

        template<typename T_schema, typename T_table>
        struct make_fields_impl<
            mp::list<T_schema, T_table>,
            mp::enable_if_c<
                    schema::is_schema<mp::decay_t<T_schema>>::value
                &&  schema::is_table <mp::decay_t<T_table >>::value>>
        {
            template<typename T_index>
            static constexpr void emplace(fields_t& fields, const T_schema& schema, const T_table& table, T_index&& index)
            {
                decltype(auto) field = make_field(schema, table, table.fields[index]);
                using field_type = mp::decay_t<decltype(field)>;
                fields.emplace_back(new field_type(std::move(field)));
            }

            template<size_t... I>
            static auto helper(const T_schema& schema, const T_table& table, std::index_sequence<I...>&&)
            {
                fields_t fields;
                int dummy[] = {0, (emplace(fields, schema, table, hana::size_c<I>), void(), 0)...};
                (void) dummy;
                return fields;
            }

            static constexpr decltype(auto) apply(const T_schema& schema, const T_table& table)
            {
                using size = decltype(hana::size(table.fields));
                return helper(schema, table, std::make_index_sequence<size::value> { });
            }
        };

    }

}
end_namespace_cpphibernate_driver_mariadb