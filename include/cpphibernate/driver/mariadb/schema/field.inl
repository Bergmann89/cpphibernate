#pragma once

#include <cpphibernate/driver/mariadb/schema/field.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* value_field_t */

    template<typename T_schema, typename T_field>
    std::string value_field_t<T_schema, T_field>::type() const
        { return type_props::type(); }

    /* primary_key_field_t */

    template<typename T_schema, typename T_field>
    std::string primary_key_field_t<T_schema, T_field>::create_table_arguments() const
            { return key_props::create_table_argument; }

}
end_namespace_cpphibernate_driver_mariadb