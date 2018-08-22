#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/mariadb.h>

beg_namespace_cpphibernate_driver
{
    
    using mariadb = mariadb_impl::mariadb_driver_t;

}
end_namespace_cpphibernate_driver