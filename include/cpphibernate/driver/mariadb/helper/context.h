#pragma once

#include <cppmariadb.h>
#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/schema/field.fwd.h>
#include <cpphibernate/driver/mariadb/schema/table.fwd.h>
#include <cpphibernate/driver/mariadb/schema/filter.fwd.h>
#include <cpphibernate/driver/mariadb/schema/schema.fwd.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* base_context */

    struct base_context
    {
        const schema_t&             schema;
        ::cppmariadb::connection&   connection;

        inline base_context(
                const schema_t&             p_schema,
                ::cppmariadb::connection&   p_connection)
            : schema    (p_schema)
            , connection(p_connection)
            { }
    };


    /* init_context */

    struct init_context
        : public base_context
    {
        bool recreate;

        inline init_context(
                const schema_t&             p_schema,
                ::cppmariadb::connection&   p_connection,
                bool                        p_recreate)
            : base_context  (p_schema, p_connection)
            , recreate      (p_recreate)
            { }
    };

    namespace __impl
    {
        struct change_context_impl
        {
            template<typename T_context, typename T_data>
            constexpr decltype(auto) operator()(const T_context& context, T_data& data) const
            {
                auto new_context    = context;
                new_context.data_id = misc::get_type_id(hana::type_c<mp::decay_t<T_data>>);
                new_context.data    = &data;
                return new_context;
            }
        };
    }

    constexpr decltype(auto) change_context = __impl::change_context_impl { };

    /* data_context */

    struct data_context
        : public base_context
    {
    private:
        friend __impl::change_context_impl;

        size_t data_id;
        void*  data;

    public:
        template<typename T>
        inline decltype(auto) get() const
        {
            if (!data)
                throw misc::hibernate_exception("no data assigned!");
            auto type_id = misc::get_type_id(hana::type_c<mp::decay_t<T>>);
            if (type_id != data_id)
            {
                throw misc::hibernate_exception(static_cast<std::ostringstream&>(std::ostringstream { }
                    << "invalid type! expected " << data_id << ", got " << type_id).str());
            }
            return *static_cast<T*>(data);
        }

        template<typename T_data>
        inline data_context(
                T_data&                     p_data,
                const schema_t&             p_schema,
                ::cppmariadb::connection&   p_connection)
            : base_context  (p_schema, p_connection)
            , data_id       (misc::get_type_id(hana::type_c<mp::decay_t<T_data>>))
            , data          (&p_data)
            { }
    };

    /* filter_context */

    struct filter_context
        : public data_context
    {
        const filter_t& filter;

        template<typename T_data>
        inline filter_context(
                T_data&                     p_data,
                const schema_t&             p_schema,
                ::cppmariadb::connection&   p_connection,
                const filter_t&             p_filter)
            : data_context  (p_data, p_schema, p_connection)
            , filter        (p_filter)
            { }
    };

    /* create_update_context */

    struct create_update_context
        : public filter_context
    {
        bool            is_update;
        const table_t*  derived_table;
        const field_t*  owner_field;
        std::string     owner_key;
        ssize_t         index;

        template<typename T_data>
        inline create_update_context(
                T_data&                     p_data,
                const schema_t&             p_schema,
                ::cppmariadb::connection&   p_connection,
                const filter_t&             p_filter,
                bool                        p_is_update)
            : filter_context(p_data, p_schema, p_connection, p_filter)
            , is_update     (p_is_update)
            , derived_table (nullptr)
            , owner_field   (nullptr)
            , index         (-1)
            { }
    };

}
end_namespace_cpphibernate_driver_mariadb