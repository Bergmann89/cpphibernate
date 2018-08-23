#pragma once

#include <cppmariadb.h>
#include <cpphibernate/config.h>
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

}
end_namespace_cpphibernate_driver_mariadb