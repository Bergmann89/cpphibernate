#pragma once

#include <vector>

#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/schema/table.h>
#include <cpphibernate/schema/schema.h>
#include <cpphibernate/driver/mariadb/schema/field.h>
#include <cpphibernate/driver/mariadb/schema/fields.fwd.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* fields_t */

    struct fields_t
        : public std::vector<field_ptr_t>
    {
        using base_type = std::vector<field_ptr_t>;
        using base_type::base_type;
    };

}
end_namespace_cpphibernate_driver_mariadb