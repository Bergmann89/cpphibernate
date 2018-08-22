#pragma once

#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/schema/tables.h>
#include <cpphibernate/driver/mariadb/schema/schema.fwd.h>

beg_namespace_cpphibernate_driver_mariadb
{
    
    /* schema_t */

    struct schema_t
    {
        std::string     schema_name;
        tables_t        tables;

        inline schema_t() = default;
        inline schema_t(const schema_t&) = delete;
        inline schema_t(schema_t&& other)
            : schema_name(std::move(other).schema_name)
            , tables     (std::move(other).tables)
            { update(); }

        void update ();
        void print  (std::ostream& os) const;
    };

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