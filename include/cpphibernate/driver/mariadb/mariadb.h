#pragma once

#include <cppmariadb.h>
#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/impl.h>
#include <cpphibernate/driver/mariadb/schema.h>

beg_namespace_cpphibernate_driver_mariadb
{

    struct mariadb_driver_t
    {
    private:
        ::cppmariadb::connection&   _connection;
        schema_t                    _schema;

    public:
        template<typename T_schema>
        mariadb_driver_t(T_schema&& p_schema, ::cppmariadb::connection& p_connection)
            : _connection   (p_connection)
            , _schema       (make_schema(std::forward<T_schema>(p_schema)))
            { }

        cpphibernate_copyable(mariadb_driver_t, delete);
        cpphibernate_moveable(mariadb_driver_t, default);

    protected:
        inline void init_impl(bool recreate) const
        {
            make_init_impl(init_context
            {
                _schema,
                _connection,
                recreate
            })();
        }
    };

}
end_namespace_cpphibernate_driver_mariadb