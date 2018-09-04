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
        size_t          id                          { 0 };          // unique id of the field
        size_t          dataset_id                  { 0 };          // unique id of the dataset type
        size_t          real_dataset_id             { 0 };          // unique id of the real/unwrapped dataset type
        size_t          value_id                    { 0 };          // unique id of the value type
        size_t          real_value_id               { 0 };          // unique id of the real/unwrapped value type

        bool            value_is_nullable           { false };      // value is stored in a nullable container
        bool            value_is_container          { false };      // value is stored in a container
        bool            value_is_ordered            { false };      // value is stored in a ordered container (vector, list, ...)
        bool            value_is_auto_incremented   { false };      // value is a auto incremented field

        const table_t*  table                       { nullptr };    // table this field belongs to
        const table_t*  referenced_table            { nullptr };    // table that belongs to the value (if exists)

        std::string     schema_name;                                // name of the SQL schema
        std::string     table_name;                                 // name of the SQL table
        std::string     field_name;                                 // name of the SQL field
        std::string     type;                                       // SQL type name
        std::string     create_arguments;                           // additional arguments for CREATE TABLE command

        std::string     convert_to_open;                            // SQL code to open the "convert to" operation
        std::string     convert_to_close;                           // SQL code to close the "convert to" operation
        std::string     convert_from_open;                          // SQL code to open the "convert from" operation
        std::string     convert_from_close;                         // SQL code to close the "convert from" operation

        attributes_t    attributes;                                 // attributes for the field

        inline field_t() = default;
        inline field_t(const field_t&) = delete;
        inline field_t(field_t&& other)
            : id                        (std::move(other).id)
            , dataset_id                (std::move(other).dataset_id)
            , real_dataset_id           (std::move(other).real_dataset_id)
            , value_id                  (std::move(other).value_id)
            , real_value_id             (std::move(other).real_value_id)
            , value_is_nullable         (std::move(other).value_is_nullable)
            , value_is_container        (std::move(other).value_is_container)
            , value_is_auto_incremented (std::move(other).value_is_auto_incremented)
            , table                     (nullptr)
            , referenced_table          (nullptr)
            , schema_name               (std::move(other).schema_name)
            , table_name                (std::move(other).table_name)
            , field_name                (std::move(other).field_name)
            , type                      (std::move(other).type)
            , create_arguments          (std::move(other).create_arguments)
            , convert_to_open           (std::move(other).convert_to_open)
            , convert_to_close          (std::move(other).convert_to_close)
            , convert_from_open         (std::move(other).convert_from_open)
            , convert_from_close        (std::move(other).convert_from_close)
            , attributes                (std::move(other).attributes)
            { }
        virtual ~field_t() { };

                void        print                   (std::ostream& os) const;
        virtual void        update                  ();

        /* CRUD */
        using read_context_ptr = std::unique_ptr<read_context>;

        virtual value_t          foreign_create_update  (const create_update_context& context) const;
        virtual read_context_ptr foreign_read           (const read_context& context, const value_t& value) const;

        /* properties */
        virtual value_t     get                     (const data_context& context) const;
        virtual void        set                     (const data_context& context, const value_t&) const;
        virtual bool        is_default              (const data_context& context) const;
        virtual std::string generate_value          (::cppmariadb::connection& connection) const;
    };

    /* simple_field_t */

    template<typename T_field>
    struct simple_field_t
        : public field_t
    {
        using base_type         = field_t;
        using field_type        = T_field;
        using getter_type       = typename mp::decay_t<field_type>::getter_type;
        using dataset_type      = typename getter_type::dataset_type;
        using real_dataset_type = misc::real_dataset_t<dataset_type>;
        using value_type        = typename getter_type::value_type;
        using real_value_type   = misc::real_dataset_t<value_type>;
        using type_props        = type_properties<value_type>;

        const field_type& field;

        inline simple_field_t(const field_type& p_field)
            : field_t   ()
            , field     (p_field)
            { }

        virtual void update() override;
    };

    /* value_field_t */

    template<typename T_field>
    struct value_field_t
        : public simple_field_t<T_field>
    {
        using base_type         = simple_field_t<T_field>;
        using field_type        = typename base_type::field_type;
        using getter_type       = typename base_type::getter_type;
        using dataset_type      = typename base_type::dataset_type;
        using real_dataset_type = typename base_type::dataset_type;
        using value_type        = typename base_type::value_type;
        using real_value_type   = typename base_type::real_value_type;
        using type_props        = typename base_type::type_props;

        using base_type::base_type;

        static_assert(mp::is_same<dataset_type, real_dataset_type>::value, "internal error: dataset type mismatch!");

        virtual void    update  () override;
        virtual value_t get     (const data_context& context) const override;
        virtual void    set     (const data_context& context, const value_t& value) const override;
    };

    /* primary_key_field_t */

    template<typename T_field>
    struct primary_key_field_t
        : public value_field_t<T_field>
    {
        using base_type     = value_field_t<T_field>;
        using field_type    = typename base_type::field_type;
        using dataset_type  = typename base_type::dataset_type;
        using value_type    = typename base_type::value_type;
        using key_props     = key_properties<value_type>;

        using base_type::base_type;

        virtual bool        is_default    (const data_context& context) const override;
        virtual std::string generate_value(::cppmariadb::connection& connection) const override;
    };

    /* data_field_t */

    template<typename T_field>
    struct data_field_t
        : public value_field_t<T_field>
    {
        using base_type = value_field_t<T_field>;

        using base_type::base_type;
    };

    /* foreign_table_field_t */

    template<typename T_field>
    struct foreign_table_field_t
        : public simple_field_t<T_field>
    {
    public:
        using base_type         = simple_field_t<T_field>;
        using value_type        = typename base_type::value_type;
        using real_value_type   = typename base_type::real_value_type;
        using dataset_type      = typename base_type::dataset_type;

        using base_type::base_type;

    public:
        virtual value_t          foreign_create_update(const create_update_context& context) const override;
        virtual read_context_ptr foreign_read         (const read_context& context, const value_t& value) const override;
    };

}
end_namespace_cpphibernate_driver_mariadb