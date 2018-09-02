#pragma once

#include <cppmariadb.h>
#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/impl.h>
#include <cpphibernate/driver/mariadb/schema.h>
#include <cpphibernate/driver/mariadb/schema/filter.h>

beg_namespace_cpphibernate_driver_mariadb
{

    struct mariadb_driver_t
    {
    private:
        ::cppmariadb::connection&   _connection;
        schema_t                    _schema;
        filter_t                    _filter;

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
            transaction_lock trans(_connection);
            _schema.init(init_context(_schema, _connection, recreate));
            trans.commit();
        }

        template<typename T_dataset>
        inline void create_impl(T_dataset& dataset) const
        {
            create_update_impl_t<T_dataset>::apply(
                create_update_context(dataset, _schema, _connection, _filter, false));
        }

        template<typename T_dataset, typename T_modifiers>
        inline void read_impl(T_dataset& dataset, T_modifiers&& modifiers) const
        {
            auto& where = build_where(_schema, modifiers);
            auto& limit = build_limit(modifiers);
            std::cout << "WHERE = " << where.query(_connection) << std::endl;
            std::cout << "LIMIT = " << limit.query(_connection) << std::endl;
        }
    };

}
end_namespace_cpphibernate_driver_mariadb