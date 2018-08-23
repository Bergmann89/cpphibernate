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

        /* properties */
        virtual std::string type                    () const;
        virtual std::string create_table_arguments  () const;
    };

    /* simple_field_t */

    template<typename T_schema, typename T_field>
    struct simple_field_t
        : public field_t
    {
        using schema_type = T_schema;
        using field_type  = T_field;

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
        using base_type         = simple_field_t<T_schema, T_field>;
        using schema_type       = T_schema;
        using field_type        = T_field;
        using getter_type       = typename mp::decay_t<field_type>::getter_type;
        using dataset_type      = typename getter_type::dataset_type;
        using value_type        = typename getter_type::value_type;
        using type_props        = type_properties<value_type>;

        using base_type::base_type;

        virtual std::string type() const override;
    };

    /* primary_key_field_t */

    template<typename T_schema, typename T_field>
    struct primary_key_field_t
        : public value_field_t<T_schema, T_field>
    {
        using base_type     = value_field_t<T_schema, T_field>;
        using schema_type   = typename base_type::schema_type;
        using field_type    = typename base_type::field_type;
        using value_type    = typename base_type::value_type;
        using key_props     = key_properties<value_type>;

        using base_type::base_type;

        virtual std::string create_table_arguments() const override;
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
        using base_type = simple_field_t<T_schema, T_field>;

        using base_type::base_type;
    };

    namespace __impl
    {

        /* is_primary_key_field */

        template<typename T_field>
        struct is_primary_key_field
            : public mp::decay_t<decltype(
                hana::contains(
                    std::declval<T_field>().attributes,
                    schema::attribute::primary_key))>
            { };

        /* is_foreign_table_field */

        template<typename T_schema, typename T_field>
        struct is_foreign_table_field
            : public mp::decay_t<decltype(
                hana::contains(
                    hana::transform(
                        std::declval<T_schema>().tables,
                        schema::table::get_wrapped_dataset),
                    hana::type_c<misc::real_dataset_t<typename T_field::getter_type::value_type>>))>
            { };

        /* field_type */

        template<typename T_schema, typename T_field, typename = void>
        struct field_type
            { using type = data_field_t<T_schema, T_field>; };

        template<typename T_schema, typename T_field>
        struct field_type<T_schema, T_field, mp::enable_if<is_primary_key_field<T_field>>>
            { using type = primary_key_field_t<T_schema, T_field>; };

        template<typename T_schema, typename T_field>
        struct field_type<T_schema, T_field, mp::enable_if<is_foreign_table_field<T_schema, T_field>>>
            { using type = foreign_table_field_t<T_schema, T_field>; };

        template<typename T_schema, typename T_field>
        using field_type_t = typename field_type<T_schema, T_field>::type;

        /* make_field_impl */

        template<typename T, typename>
        struct make_field_impl
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for mariadb::make_field(...)!"); }
        };

        template<typename T_schema, typename T_table, typename T_field>
        struct make_field_impl<
            mp::list<T_schema, T_table, T_field>,
            mp::enable_if_c<
                    schema::is_schema<mp::decay_t<T_schema>>::value
                &&  schema::is_table <mp::decay_t<T_table >>::value
                &&  schema::is_field <mp::decay_t<T_field >>::value>>
        {
            static constexpr decltype(auto) apply(const T_schema& schema, const T_table& table, const T_field& field)
            {
                using schema_type           = mp::decay_t<T_schema>;
                using field_type            = mp::decay_t<T_field>;
                using getter_type           = mp::decay_t<typename field_type::getter_type>;
                using value_type            = mp::decay_t<typename getter_type::value_type>;
                using dataset_type          = mp::decay_t<typename getter_type::dataset_type>;
                using value_dataset_type    = misc::real_dataset_t<value_type>;
                using return_type           = field_type_t<schema_type, field_type>;
                using primary_key_type      = primary_key_field_t<schema_type, field_type>;
                return_type ret(schema, field);
                ret.table_dataset_id   = misc::get_type_id(hana::type_c<dataset_type>);
                ret.value_dataset_id   = misc::get_type_id(hana::type_c<value_dataset_type>);
                ret.value_is_nullable  = misc::is_nullable<value_type>::value;
                ret.value_is_container = misc::is_container<value_type>::value;
                ret.schema_name        = schema.name;
                ret.table_name         = table.name;
                ret.field_name         = field.name;
                ret.attributes         = make_attributes(field.attributes);
                hana::eval_if(
                    hana::equal(hana::type_c<return_type>, hana::type_c<primary_key_type>),
                    [&ret](){
                        ret.field_name = ret.table_name + "_" + ret.field_name;
                    }, [](){ });
                return ret;
            }
        };

    }

}
end_namespace_cpphibernate_driver_mariadb