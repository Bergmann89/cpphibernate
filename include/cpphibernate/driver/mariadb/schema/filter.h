#pragma once

#include <set>
#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/schema/field.fwd.h>
#include <cpphibernate/driver/mariadb/schema/table.fwd.h>
#include <cpphibernate/driver/mariadb/schema/filter.fwd.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* filter_t */

    struct filter_t
    {
        using field_set_type = std::set<const field_t*>;
        using table_set_type = std::set<const table_t*>;

        size_t          cache_id { 0 };
        field_set_type  fields;
        table_set_type  tables;

        inline bool is_excluded(const table_t& table) const
            { return tables.count(&table); }

        inline bool is_excluded(const field_t& field) const
            { return fields.count(&field); }
    };

}
end_namespace_cpphibernate_driver_mariadb