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

    /* create_update_context */

    struct create_update_context
    {
        bool                        is_update;
        const schema_t&             schema;
        const filter_t&             filter;
        const table_t*              derived_table;
        const field_t*              owner_field;
        ::cppmariadb::connection&   connection;
    };

}
end_namespace_cpphibernate_driver_mariadb