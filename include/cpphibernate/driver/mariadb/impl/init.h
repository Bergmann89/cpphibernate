#pragma once

#include <cppmariadb.h>
#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/helper.h>
#include <cpphibernate/driver/mariadb/schema.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* init_impl_t */

    template<typename T_context, typename = void>
    struct init_impl_t
    {
        using context_type = T_context;

        static inline void apply(const context_type& context)
        {
            auto& schema        = context.schema;
            auto& connection    = context.connection;

            transaction_lock trans(connection);
            schema.init(context);
            trans.commit();
        }
    };

}
end_namespace_cpphibernate_driver_mariadb