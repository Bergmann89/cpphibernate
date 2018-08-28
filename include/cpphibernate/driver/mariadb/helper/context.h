#pragma once

#include <cppmariadb.h>
#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/schema/field.fwd.h>
#include <cpphibernate/driver/mariadb/schema/table.fwd.h>
#include <cpphibernate/driver/mariadb/schema/filter.fwd.h>
#include <cpphibernate/driver/mariadb/schema/schema.fwd.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* init_context */

    struct init_context
    {
        const schema_t&             schema;
        ::cppmariadb::connection&   connection;
        bool                        recreate;
    };

    /* create context */

    struct create_context
    {
        const schema_t&             schema;
        const table_t*              derived_table;
        const field_t*              owner_field;
        ::cppmariadb::connection&   connection;
    };

    template<typename T_dataset>
    struct generic_create_context
        : public create_context
    {
        using dataset_type = mp::decay_t<T_dataset>;

        dataset_type& dataset;

        template<typename T_new_dataset>
        constexpr decltype(auto) change(T_new_dataset& new_dataset, const field_t* owner = nullptr) const
        {
            return generic_create_context<T_new_dataset>
            {
                {
                    schema,
                    nullptr,
                    owner,
                    connection
                },
                new_dataset
            };
        }
    };

    /* update context */

    struct update_context
        : public create_context
    {
        const filter_t& filter;
    };

    template<typename T_dataset>
    struct generic_update_context
        : public update_context
    {
        using dataset_type = mp::decay_t<T_dataset>;

        dataset_type& dataset;

        template<typename T_new_dataset>
        constexpr decltype(auto) change(T_new_dataset& new_dataset, const field_t* owner = nullptr) const
        {
            return generic_update_context<T_new_dataset>
            {
                {
                    {
                        schema,
                        nullptr,
                        owner,
                        connection
                    },
                    filter,
                },
                new_dataset
            };
        }
    };

}
end_namespace_cpphibernate_driver_mariadb