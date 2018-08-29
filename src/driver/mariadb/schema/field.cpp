#include <string>
#include <iostream>

#include <cpputils/misc/enum.h>
#include <cpputils/misc/string.h>
#include <cpputils/misc/indent.h>

#include <cpphibernate/misc.h>
#include <cpphibernate/driver/mariadb/schema/field.h>
#include <cpphibernate/driver/mariadb/schema/table.h>

using namespace ::std;
using namespace ::utl;
using namespace ::cpphibernate::driver::mariadb_impl;

void field_t::print(std::ostream& os) const
{
    os  << indent << '{'
        << incindent
            << indent << "\"table_dataset_id\": "   <<  table_dataset_id << ","
            << indent << "\"value_dataset_id\": "   <<  value_dataset_id << ","
            << indent << "\"value_is_nullable\": "  <<  (value_is_nullable ? "true" : "false") << ","
            << indent << "\"value_is_container\": " <<  (value_is_container ? "true" : "false") << ","
            << indent << "\"schema_name\": \""      <<  schema_name << "\","
            << indent << "\"table_name\": \""       <<  table_name << "\","
            << indent << "\"field_name\": \""       <<  field_name << "\","
            << indent << "\"attributes\": "         <<  misc::print_container(attributes, false) << ","
            << indent << "\"table\": "              << (table ? std::string("\"") + table->table_name + "\"" : "null") << ","
            << indent << "\"referenced_table\": "   << (referenced_table ? std::string("\"") + referenced_table->table_name + "\"" : "null")
        << decindent
        << indent << '}';
}

#define throw_not_implemented(p_ret, p_name, ...)               \
    p_ret field_t::p_name(__VA_ARGS__) const                    \
    {                                                           \
        throw misc::hibernate_exception(                        \
            std::string("'") + table_name + "." + field_name +  \
            "' does not implement the " #p_name "() method!");  \
    }

/* CRUD */

throw_not_implemented(value_t,  foreign_create_update, const create_update_context&)

/* properties */

throw_not_implemented(bool,     is_default, const data_context& context)
throw_not_implemented(string,   type)
throw_not_implemented(string,   create_table_arguments)
throw_not_implemented(string,   generate_value, ::cppmariadb::connection&)
throw_not_implemented(value_t,  get, const data_context& context)
throw_not_implemented(void,     set, const data_context& context, const value_t&)

bool field_t::is_auto_generated() const
    { return false; }

std::string field_t::convert_to_open() const
{
    std::ostringstream ss;
    for (auto it = this->attributes.begin(); it != this->attributes.end(); ++it)
    {
        switch(*it)
        {
            case attribute_t::hex:         ss << "HEX(";       break;
            case attribute_t::compress:    ss << "COMPRESS(";  break;
            case attribute_t::primary_key:                     break;
        }
    }
    return ss.str();
}

std::string field_t::convert_to_close() const
{
    std::ostringstream ss;
    for (auto it = this->attributes.begin(); it != this->attributes.end(); ++it)
    {
        switch(*it)
        {
            case attribute_t::hex:
            case attribute_t::compress:
                ss << ')';
                break;
            case attribute_t::primary_key:
                break;
        }
    }
    return ss.str();
}

std::string field_t::convert_from_open() const
{
    std::ostringstream ss;
    for (auto it = this->attributes.rbegin(); it != this->attributes.rend(); ++it)
    {
        switch(*it)
        {
            case attribute_t::hex:
                ss << "UNHEX(";
                break;
            case attribute_t::compress:
                ss << "UNCOMPRESS(";
                break;
            case attribute_t::primary_key:
                break;
        }
    }
    return ss.str();
}

std::string field_t::convert_from_close() const
{
    std::ostringstream ss;
    for (auto it = this->attributes.rbegin(); it != this->attributes.rend(); ++it)
    {
        switch(*it)
        {
            case attribute_t::hex:
            case attribute_t::compress:
                ss << ')';
                break;
            case attribute_t::primary_key:
                break;
        }
    }
    return ss.str();
}