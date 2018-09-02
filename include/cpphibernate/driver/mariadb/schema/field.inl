#pragma once

#include <cpphibernate/driver/mariadb/schema/field.h>
#include <cpphibernate/driver/mariadb/impl/create_update.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* simple_field_t */

    template<typename T_field>
    void simple_field_t<T_field>
        ::update()
    {
        base_type::update();

        id              = misc::get_type_id(hana::type_c<field_type>);
        dataset_id      = misc::get_type_id(hana::type_c<dataset_type>);
        real_dataset_id = misc::get_type_id(hana::type_c<real_dataset_type>);
        value_id        = misc::get_type_id(hana::type_c<value_type>);
        real_value_id   = misc::get_type_id(hana::type_c<real_value_type>);

        value_is_nullable  = misc::is_nullable<value_type>::value;
        value_is_container = misc::is_container<value_type>::value;
        value_is_ordered   = misc::is_ordered<value_type>::value;
    }

    /* value_field_t */

    template<typename T_field>
    void value_field_t<T_field>
        ::update()
    {
        base_type::update();
        this->type = type_props::type();
    }

    template<typename T_field>
    value_t value_field_t<T_field>
        ::get(const data_context& context) const
    {
        auto& dataset = context.get<dataset_type>(this->table);
        return type_props::convert_from(this->field.getter(dataset));
    }

    template<typename T_field>
    void value_field_t<T_field>
        ::set(const data_context& context, const value_t& value) const
    {
        auto& dataset = context.get<dataset_type>(this->table);
        this->field.setter(dataset, type_props::convert_to(value));
    }

    /* primary_key_field_t */

    template<typename T_field>
    bool primary_key_field_t<T_field>
        ::is_default(const data_context& context) const
    {
        auto& dataset = context.get<dataset_type>();
        return key_props::is_default(this->field.getter(dataset));
    }

    template<typename T_field>
    std::string primary_key_field_t<T_field>
        ::generate_value(::cppmariadb::connection& connection) const
    {
        auto ret = connection.execute_used(key_props::create_key_query);
        if (!ret || !ret->next())
            throw misc::hibernate_exception("unable to generate key value!");
        return ret->current()->at(0).template get<std::string>();
    }

    /* foreign_table_field_t */

    template<typename T_field>
    value_t foreign_table_field_t<T_field>
        ::foreign_create_update(const create_update_context& context) const
    {
        auto& dataset       = context.get<dataset_type>();
        auto& foreign       = this->field.getter(dataset);
        auto  next_context  = change_context(context, foreign);

        using foreign_dataset_type = mp::decay_t<decltype(foreign)>;
        return create_update_impl_t<foreign_dataset_type>::apply(
            next_context,
            false);
    }

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
            { using type = data_field_t<T_field>; };

        template<typename T_schema, typename T_field>
        struct field_type<T_schema, T_field, mp::enable_if<is_primary_key_field<T_field>>>
            { using type = primary_key_field_t<T_field>; };

        template<typename T_schema, typename T_field>
        struct field_type<T_schema, T_field, mp::enable_if<is_foreign_table_field<T_schema, T_field>>>
            { using type = foreign_table_field_t<T_field>; };

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
                using return_type           = field_type_t<schema_type, field_type>;
                using primary_key_type      = primary_key_field_t<field_type>;
                return_type ret(field);
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