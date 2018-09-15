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
            using dataset_type      = mp::decay_t<T_dataset>;
            using real_dataset_type = misc::real_dataset_t<dataset_type>;

            auto  dataset_id = misc::get_type_id(hana::type_c<real_dataset_type>);
            auto& table      = _schema.table(dataset_id);
            auto  context    = make_read_context(dataset, _schema, _connection, _filter);
            context.where    = build_where(_schema, modifiers).query(_connection);
            context.limit    = build_limit(modifiers).query(_connection);
            context.order_by = build_order_by(_schema, modifiers).query(_connection);

            transaction_lock trans(_connection);
            table.read(context);
            trans.commit();
        }

        template<typename T_dataset>
        inline void update_impl(T_dataset& dataset) const
        {
            create_update_impl_t<T_dataset>::apply(
                create_update_context(dataset, _schema, _connection, _filter, true));
        }
    };

}
end_namespace_cpphibernate_driver_mariadb