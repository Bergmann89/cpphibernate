#include <cpphibernate/driver/mariadb/schema/table.h>
#include <cpphibernate/driver/mariadb/schema/filter.h>

using namespace ::cpphibernate::driver::mariadb_impl;

bool filter_t::contains(const table_t* table, bool check_base) const
{
    if (tables.count(table))
        return true;
    else if (check_base && table->base_table)
        return contains(table->base_table, true);
    else
        return false;
}

bool filter_t::contains(const field_t* field) const
{
    return (fields.count(field) > 0);
}