#pragma once

#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/schema/field.h>
#include <cpphibernate/schema/table.h>
#include <cpphibernate/schema/schema.h>
#include <cpphibernate/driver/mariadb/helper.h>
#include <cpphibernate/driver/mariadb/schema/field.fwd.h>
#include <cpphibernate/driver/mariadb/schema/table.fwd.h>
#include <cpphibernate/driver/mariadb/schema/attributes.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* field_t */

    struct field_t
    {
        size_t          table_dataset_id    { 0 };
        size_t          value_dataset_id    { 0 };
        bool            value_is_nullable   { false };
        bool            value_is_container  { false };
        std::string     schema_name;
        std::string     table_name;
        std::string     field_name;
        attributes_t    attributes;

        const table_t*  table               { nullptr };
        const table_t*  referenced_table    { nullptr };

        inline field_t() = default;
        inline field_t(const field_t&) = delete;
        inline field_t(field_t&& other)
            : table_dataset_id  (std::move(other).table_dataset_id)
            , value_dataset_id  (std::move(other).value_dataset_id)
            , value_is_nullable (std::move(other).value_is_nullable)
            , value_is_container(std::move(other).value_is_container)
            , schema_name       (std::move(other).schema_name)
            , table_name        (std::move(other).table_name)
            , field_name        (std::move(other).field_name)
            , attributes        (std::move(other).attributes)
            , table             (nullptr)
            , referenced_table  (nullptr)
            { }
        virtual ~field_t() { };

        void print(std::ostream& os) const;

        /* CRUD */
        virtual value_t foreign_create_update(const create_update_context& context) const;

        /* properties */
        virtual std::string type                    () const;
        virtual std::string create_table_arguments  () const;
        virtual std::string generate_value          (::cppmariadb::connection& connection) const;
        virtual bool        is_auto_generated       () const;
        virtual bool        is_default              (const data_context& context) const;
        virtual std::string convert_to_open         () const;
        virtual std::string convert_to_close        () const;
        virtual std::string convert_from_open       () const;
        virtual std::string convert_from_close      () const;
        virtual value_t     get                     (const data_context& context) const;
        virtual void        set                     (const data_context& context, const value_t&) const;
    };

    /* simple_field_t */

    template<typename T_schema, typename T_field>
    struct simple_field_t
        : public field_t
    {
        using schema_type   = T_schema;
        using field_type    = T_field;
        using getter_type   = typename mp::decay_t<field_type>::getter_type;
        using dataset_type  = typename getter_type::dataset_type;
        using value_type    = typename getter_type::value_type;

        const schema_type&  schema;
        const field_type&   field;

        inline simple_field_t(const schema_type& p_schema, const field_type& p_field)
            : field_t   ()
            , schema    (p_schema)
            , field     (p_field)
            { }
    };

    /* value_field_t */

    template<typename T_schema, typename T_field>
    struct value_field_t
        : public simple_field_t<T_schema, T_field>
    {
        using base_type     = simple_field_t<T_schema, T_field>;
        using schema_type   = T_schema;
        using field_type    = T_field;
        using getter_type   = typename base_type::getter_type;
        using dataset_type  = typename base_type::dataset_type;
        using value_type    = typename base_type::value_type;
        using type_props    = type_properties<value_type>;

        using base_type::base_type;

        virtual std::string type() const override;
        virtual value_t     get (const data_context& context) const override;
        virtual void        set (const data_context& context, const value_t&) const override;
    };

    /* primary_key_field_t */

    template<typename T_schema, typename T_field>
    struct primary_key_field_t
        : public value_field_t<T_schema, T_field>
    {
        using base_type     = value_field_t<T_schema, T_field>;
        using schema_type   = typename base_type::schema_type;
        using field_type    = typename base_type::field_type;
        using dataset_type  = typename base_type::dataset_type;
        using value_type    = typename base_type::value_type;
        using key_props     = key_properties<value_type>;

        using base_type::base_type;

        virtual std::string create_table_arguments  () const override;
        virtual std::string generate_value          (::cppmariadb::connection& connection) const override;
        virtual bool        is_auto_generated       () const override;
        virtual bool        is_default              (const data_context& context) const override;
        virtual std::string convert_to_open         () const override;
        virtual std::string convert_to_close        () const override;
        virtual std::string convert_from_open       () const override;
        virtual std::string convert_from_close      () const override;
    };

    /* data_field_t */

    template<typename T_schema, typename T_field>
    struct data_field_t
        : public value_field_t<T_schema, T_field>
    {
        using base_type = value_field_t<T_schema, T_field>;

        using base_type::base_type;
    };

    /* foreign_table_field_t */

    template<typename T_schema, typename T_field>
    struct foreign_table_field_t
        : public simple_field_t<T_schema, T_field>
    {
    public:
        using base_type     = simple_field_t<T_schema, T_field>;
        using dataset_type  = typename base_type::dataset_type;

        using base_type::base_type;

    public:
        virtual value_t foreign_create_update(const create_update_context& context) const override;
    };

}
end_namespace_cpphibernate_driver_mariadb