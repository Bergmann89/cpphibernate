#pragma once

#include <cpphibernate/driver/mariadb/schema/schema.h>

beg_namespace_cpphibernate_driver_mariadb
{

    namespace __impl
    {

        /* make_schema_impl */

        template<typename T, typename>
        struct make_schema_impl
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for mariadb::make_schema(...)!"); }
        };

        template<typename T_schema>
        struct make_schema_impl<
            mp::list<T_schema>,
            mp::enable_if_c<
                schema::is_schema<mp::decay_t<T_schema>>::value>>
        {
            static decltype(auto) apply(const T_schema& schema)
            {
                schema_t ret;
                ret.schema_name = schema.name;
                ret.tables      = make_tables(schema);
                ret.update();
                return ret;
            }
        };

    }

}
end_namespace_cpphibernate_driver_mariadb