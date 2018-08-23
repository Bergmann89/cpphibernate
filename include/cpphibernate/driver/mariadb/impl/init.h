#pragma once

#include <cppmariadb.h>
#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/helper.h>
#include <cpphibernate/driver/mariadb/schema.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* init_impl */

    template<typename T_context, typename = void>
    struct init_impl
    {
        using context_type = T_context;

        context_type context;

        constexpr init_impl(T_context&& p_context)
            : context(std::forward<T_context>(p_context))
            { }

        /* operator() */

        inline void operator()() const
        {
            auto& schema        = context.schema;
            auto& connection    = context.connection;

            transaction_lock trans(connection);
            schema.init(context);
            trans.commit();
        }
    };

    /* make_init_impl */

    template<typename T_context>
    constexpr decltype(auto) make_init_impl(T_context&& context)
        { return init_impl<T_context>(std::forward<T_context>(context)); }

}
end_namespace_cpphibernate_driver_mariadb