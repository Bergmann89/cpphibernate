#pragma once

#include <map>

#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/schema/schema.h>
#include <cpphibernate/driver/mariadb/schema/table.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* tables_t */

    struct tables_t
        : public std::map<size_t, table_ptr_t>
    {
        using base_type = std::map<size_t, table_ptr_t>;
        using base_type::base_type;
    };

}
end_namespace_cpphibernate_driver_mariadb