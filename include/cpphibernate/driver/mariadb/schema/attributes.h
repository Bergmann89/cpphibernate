#pragma once

#include <set>

#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/schema/attributes.h>
#include <cpphibernate/driver/mariadb/schema/attributes.fwd.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* attributes_t */

    struct attributes_t :
        public std::set<attribute_t>
    {
        using base_type = std::set<attribute_t>;
        using base_type::base_type;
    };

}
end_namespace_cpphibernate_driver_mariadb