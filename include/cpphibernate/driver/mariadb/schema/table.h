#pragma once

#include <memory>
#include <vector>

#include <cppmariadb.h>
#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/schema/table.h>
#include <cpphibernate/schema/schema.h>
#include <cpphibernate/driver/mariadb/schema/fields.h>
#include <cpphibernate/driver/mariadb/schema/table.fwd.h>
#include <cpphibernate/driver/mariadb/schema/filter.fwd.h>
#include <cpphibernate/driver/mariadb/helper/context.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* table_t */

    struct table_t
    {
    public:
        size_t                      dataset_id          { 0 };
        size_t                      base_dataset_id     { 0 };
        size_t                      table_id            { 0 };

        std::vector<size_t>         derived_dataset_ids;
        std::string                 table_name;
        std::string                 schema_name;

        fields_t                    fields;

        const table_t*              base_table          { nullptr };
        std::vector<const table_t*> derived_tables;

        const field_t*              primary_key_field   { nullptr };
        std::vector<const field_t*> foreign_key_fields;
        std::vector<const field_t*> foreign_table_fields;
        std::vector<const field_t*> foreign_table_one_fields;
        std::vector<const field_t*> foreign_table_many_fields;
        std::vector<const field_t*> data_fields;

        inline table_t() = default;
        inline table_t(const table_t&) = delete;
        inline table_t(table_t&& other)
            : dataset_id                (std::move(other).dataset_id)
            , base_dataset_id           (std::move(other).base_dataset_id)
            , table_id                  (std::move(other).table_id)
            , derived_dataset_ids       (std::move(other).derived_dataset_ids)
            , table_name                (std::move(other).table_name)
            , schema_name               (std::move(other).schema_name)
            , fields                    (std::move(other).fields)
            , base_table                (nullptr)
            , derived_tables            ()
            , primary_key_field         (nullptr)
            , foreign_key_fields        ()
            , foreign_table_fields      ()
            , foreign_table_one_fields  ()
            , foreign_table_many_fields ()
            , data_fields               ()
            { }
        virtual ~table_t() { };

        void print(std::ostream& os) const;

        const table_t* get_derived_by_table_id(size_t id) const;
        const table_t* get_derived_by_dataset_id(size_t id) const;

        virtual void       emplace              (const read_context& context) const;
               std::string get_where_primary_key(const data_context& context) const;
               std::string build_delete_query   (const std::string* where) const;

        /* CRUD */
        inline void init_stage1(const init_context& context) const
            { return init_stage1_exec(context); }

        inline void init_stage2(const init_context& context) const
            { return init_stage2_exec(context); }

        inline decltype(auto) create_update(const create_update_context& context) const
            { return create_update_intern(context); }

        inline void read(const read_context& context) const
            { return read_exec(context); }

        inline void destroy(const destroy_context& context) const
            { return destroy_intern(context); }

    private:
        template<typename T_schema, typename T_table, typename T_base_dataset>
        friend struct table_simple_t;

        template<typename T_schema, typename T_table, typename T_base_dataset>
        friend struct table_polymorphic_t;

        using statement_ptr = std::unique_ptr<::cppmariadb::statement>;
        using map_key       = std::tuple<size_t, const field_t*>;
        using statement_map = std::map<map_key, ::cppmariadb::statement>;

        mutable statement_ptr _statement_create_table;
        mutable statement_ptr _statement_alter_table;
        mutable statement_ptr _statement_insert_into;
        mutable statement_map _statement_select_static;
        mutable statement_map _statement_select_dynamic;
        mutable statement_map _statement_update;
        mutable statement_ptr _statement_foreign_many_delete;
        mutable statement_ptr _statement_delete;

        ::cppmariadb::statement& get_statement_create_table() const;
        ::cppmariadb::statement* get_statement_alter_table() const;
        ::cppmariadb::statement& get_statement_insert_into() const;
        ::cppmariadb::statement& get_statement_select(const read_context& context) const;
        ::cppmariadb::statement& get_statement_update(const filter_t& filter, const field_t* owner) const;
        ::cppmariadb::statement& get_statement_foreign_many_delete() const;
        ::cppmariadb::statement& get_statement_delete() const;

        void execute_foreign_many_delete(const base_context& context) const;

        std::string execute_create_update(
            const create_update_context&    context,
            ::cppmariadb::statement&        statement) const;

        virtual std::string create_update_base(const create_update_context& context) const;

    protected:
                void        init_stage1_exec    (const init_context& context) const;
                void        init_stage2_exec    (const init_context& context) const;

        virtual std::string create_update_intern(const create_update_context& context) const;
                std::string create_update_exec  (const create_update_context& context) const;

                void        read_exec           (const read_context& context) const;

        virtual void        destroy_intern      (const destroy_context& context) const;
                void        destroy_exec        (const destroy_context& context) const;
                void        destroy_cleanup     (const base_context& context, bool check_derived, bool check_base) const;
    };

    /* table_simple_t */

    template<typename T_schema, typename T_table, typename T_base_dataset>
    struct table_simple_t
        : public table_t
    {
    public:
        using schema_type       = T_schema;
        using table_type        = T_table;
        using base_dataset_type = T_base_dataset;
        using dataset_type      = typename table_type::dataset_type;

        const schema_type&  schema;
        const table_type&   table;

        inline table_simple_t(const schema_type& p_schema, const table_type& p_table)
            : schema(p_schema)
            , table (p_table)
            { }

        inline table_simple_t(const table_simple_t&) = delete;

        inline table_simple_t(table_simple_t&& other)
            : table_t(std::move(other))
            , schema (std::move(other).schema)
            , table  (std::move(other).table)
            { }
    };

    /* table_polymorphic_t */

    template<typename T_schema, typename T_table, typename T_base_dataset>
    struct table_polymorphic_t
        : public table_simple_t<T_schema, T_table, T_base_dataset>
    {
    public:
        using base_type         = table_simple_t<T_schema, T_table, T_base_dataset>;
        using schema_type       = typename base_type::schema_type;
        using table_type        = typename base_type::table_type;
        using base_dataset_type = typename base_type::base_dataset_type;
        using dataset_type      = typename table_type::dataset_type;
        using real_dataset_type = misc::real_dataset_t<dataset_type>;

        using base_type::base_type;

        virtual void emplace(const read_context& context) const override;

    private:
        template<typename T_dataset, typename T_pred, typename T_include_self>
        constexpr void for_each_derived(T_dataset& dataset, const T_include_self& include_self, const T_pred& pred) const;

    protected:
        virtual std::string create_update_intern(const create_update_context& context) const override;
        virtual void        destroy_intern      (const destroy_context& context) const override;

    private:
        virtual std::string create_update_base(const create_update_context& context) const override;
    };

}
end_namespace_cpphibernate_driver_mariadb