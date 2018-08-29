#pragma once

#include <cpphibernate/driver/mariadb/schema/field.h>
#include <cpphibernate/driver/mariadb/impl/create_update.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* value_field_t */

    template<typename T_schema, typename T_field>
    std::string value_field_t<T_schema, T_field>::type() const
        { return type_props::type(); }

    template<typename T_schema, typename T_field>
    value_t value_field_t<T_schema, T_field>::get() const
        { return type_props::convert_from(this->field.getter(ref_stack::top())); }

    template<typename T_schema, typename T_field>
    void value_field_t<T_schema, T_field>::set(const value_t& value) const
        { this->field.setter(ref_stack::top(), type_props::convert_to(value)); }

    /* primary_key_field_t */

    template<typename T_schema, typename T_field>
    std::string primary_key_field_t<T_schema, T_field>::create_table_arguments() const
        { return key_props::create_table_argument; }

    template<typename T_schema, typename T_field>
    std::string primary_key_field_t<T_schema, T_field>
        ::generate_value(::cppmariadb::connection& connection) const
    {
        auto ret = connection.execute_used(key_props::create_key_query);
        if (!ret || !ret->next())
            throw misc::hibernate_exception("unable to generate key value!");
        return ret->current()->at(0).template get<std::string>();
    }

    template<typename T_schema, typename T_field>
    bool primary_key_field_t<T_schema, T_field>::is_auto_generated() const
        { return key_props::auto_generated::value; }

    template<typename T_schema, typename T_field>
    bool primary_key_field_t<T_schema, T_field>::is_default() const
        { return key_props::is_default(this->field.getter(ref_stack::top())); }

    template<typename T_schema, typename T_field>
    std::string primary_key_field_t<T_schema, T_field>::convert_to_open() const
        { return key_props::convert_to_open; }

    template<typename T_schema, typename T_field>
    std::string primary_key_field_t<T_schema, T_field>::convert_to_close() const
        { return key_props::convert_to_close; }

    template<typename T_schema, typename T_field>
    std::string primary_key_field_t<T_schema, T_field>::convert_from_open() const
        { return key_props::convert_from_open; }

    template<typename T_schema, typename T_field>
    std::string primary_key_field_t<T_schema, T_field>::convert_from_close() const
        { return key_props::convert_from_close; }

    /* foreign_table_field_t */

    template<typename T_schema, typename T_field>
    value_t foreign_table_field_t<T_schema, T_field>
        ::foreign_create_update(const create_update_context& context) const
    {
        auto& ref     = ref_stack::top();
        auto& foreign = this->field.getter(ref);

        using foreign_dataset_type = mp::decay_t<decltype(foreign)>;

        return create_update_impl_t<foreign_dataset_type>::apply(
            foreign,
            context,
            false);
    }

}
end_namespace_cpphibernate_driver_mariadb