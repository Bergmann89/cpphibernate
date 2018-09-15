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
                auto new_context = context;
                new_context.set(data);
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

        mutable size_t          _dataset_id;
        mutable void*           _dataset;
        mutable const table_t*  _table;

    protected:
        template<typename T_dataset>
        inline void* set(T_dataset& dataset, size_t dataset_id = 0) const;

    public:
        template<typename T_dataset>
        inline decltype(auto) get() const;

        inline data_context(
                const schema_t&             p_schema,
                ::cppmariadb::connection&   p_connection)
            : base_context  (p_schema, p_connection)
            , _dataset_id   (0)
            , _dataset      (nullptr)
            , _table        (nullptr)
            { }

        template<typename T_data>
        inline data_context(
                T_data&                     p_data,
                const schema_t&             p_schema,
                ::cppmariadb::connection&   p_connection)
            : base_context  (p_schema, p_connection)
            , _dataset_id   (misc::get_type_id(hana::type_c<mp::decay_t<T_data>>))
            , _dataset      (&p_data)
            , _table        (nullptr)
            { }
    };

    /* filter_context */

    struct filter_context
        : public data_context
    {
        const filter_t& filter;

        inline filter_context(
                const schema_t&             p_schema,
                ::cppmariadb::connection&   p_connection,
                const filter_t&             p_filter)
            : data_context  (p_schema, p_connection)
            , filter        (p_filter)
            { }

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

    /* read_context */

    struct read_context
        : public filter_context
    {
    protected:
        size_t      base_dataset_id;

    public:
        bool        is_dynamic;
        std::string where;
        std::string limit;
        std::string order_by;

    protected:
        inline read_context(
                const schema_t&             p_schema,
                ::cppmariadb::connection&   p_connection,
                const filter_t&             p_filter)
            : filter_context    (p_schema, p_connection, p_filter)
            , is_dynamic        (false)
            , base_dataset_id   (0)
            { }

    public:
        virtual ~read_context() = default;

        template<typename T_dataset>
        inline T_dataset& emplace(const table_t* table = nullptr) const;

        void emplace() const
            { emplace_intern(nullptr, 0); }

        void finish() const
            { finish_intern(); }

    private:
        virtual void* emplace_intern(void* data, size_t dataset_id) const = 0;
        virtual void  finish_intern () const = 0;
    };

    using read_context_ptr = std::unique_ptr<read_context>;

    /* destroy_context */

    struct destroy_context
        : public data_context
    {
        std::string where;

        template<typename T_data>
        inline destroy_context(
                T_data&                     p_data,
                const schema_t&             p_schema,
                ::cppmariadb::connection&   p_connection)
            : data_context(
                p_data,
                p_schema,
                p_connection)
            { }
    };

}
end_namespace_cpphibernate_driver_mariadb