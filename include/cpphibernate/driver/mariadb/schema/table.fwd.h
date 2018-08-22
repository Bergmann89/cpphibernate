#pragma once

#include <memory>
#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>

beg_namespace_cpphibernate_driver_mariadb
{
    
    /* table_t */

    struct table_t;

    /* table_ptr_t */

    using table_ptr_t = std::unique_ptr<table_t>;

    /* make_table */

    namespace __impl
    {

        template<typename T, typename = void>
        struct make_table_impl;

    }

    constexpr decltype(auto) make_table = misc::make_generic_predicate<__impl::make_table_impl> { };

}
end_namespace_cpphibernate_driver_mariadb