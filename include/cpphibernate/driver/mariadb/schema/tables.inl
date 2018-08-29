#pragma once

#include <cpphibernate/driver/mariadb/schema/tables.h>

beg_namespace_cpphibernate_driver_mariadb
{

    namespace __impl
    {

        /* make_tables_impl */

        template<typename T, typename>
        struct make_tables_impl
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for mariadb::make_tables(...)!"); }
        };

        template<typename T_schema>
        struct make_tables_impl<
            mp::list<T_schema>,
            mp::enable_if_c<
                schema::is_schema<mp::decay_t<T_schema>>::value>>
        {
            template<typename T_index>
            static constexpr void emplace(tables_t& tables, const T_schema& schema, T_index&& index)
            {
                decltype(auto) table = make_table(schema, schema.tables[index]);
                using table_type = mp::clean_type<decltype(table)>;
                auto key = table.dataset_id;
                tables.emplace(key, std::make_unique<table_type>(std::move(table)));
            }

            template<size_t... I>
            static decltype(auto) helper(const T_schema& schema, std::index_sequence<I...>)
            {
                tables_t tables;
                int dummy[] = {0, (emplace(tables, schema, hana::size_c<I>), void(), 0)...};
                (void) dummy;
                return tables;
            }

            static constexpr decltype(auto) apply(const T_schema& schema)
            {
                using size = decltype(hana::size(schema.tables));
                return helper(schema, std::make_index_sequence<size::value> { });
            }
        };

    }

}
end_namespace_cpphibernate_driver_mariadb