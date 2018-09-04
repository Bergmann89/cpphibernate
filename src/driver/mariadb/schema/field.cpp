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

            << indent << "\"id\": "                         <<  id << ","
            << indent << "\"dataset_id\": "                 <<  dataset_id << ","
            << indent << "\"real_dataset_id\": "            <<  real_dataset_id << ","
            << indent << "\"value_id\": "                   <<  value_id << ","
            << indent << "\"real_value_id\": "              <<  real_value_id << ","
            << indent << "\"value_is_nullable\": "          <<  (value_is_nullable ? "true" : "false") << ","
            << indent << "\"value_is_container\": "         <<  (value_is_container ? "true" : "false") << ","
            << indent << "\"value_is_auto_incremented\": "  <<  (value_is_auto_incremented ? "true" : "false") << ","
            << indent << "\"table\": "                      << (table ? std::string("\"") + table->table_name + "\"" : "null") << ","
            << indent << "\"referenced_table\": "           << (referenced_table ? std::string("\"") + referenced_table->table_name + "\"" : "null") << ","

            << indent << "\"schema_name\": \""              <<  schema_name << "\","
            << indent << "\"table_name\": \""               <<  table_name << "\","
            << indent << "\"field_name\": \""               <<  field_name << "\","
            << indent << "\"type\": \""                     <<  type << "\","
            << indent << "\"create_arguments\": \""         <<  create_arguments << "\","

            << indent << "\"convert_to_open\": \""          <<  convert_to_open << "\","
            << indent << "\"convert_to_close\": \""         <<  convert_to_close << "\","
            << indent << "\"convert_from_open\": \""        <<  convert_from_open << "\","
            << indent << "\"convert_from_close\": \""       <<  convert_from_close << "\","

            << indent << "\"attributes\": "                 <<  misc::print_container(attributes, false)

        << decindent
        << indent << '}';
}

void field_t::update()
{
    id                          = 0;
    dataset_id                  = 0;
    real_dataset_id             = 0;
    value_id                    = 0;
    real_value_id               = 0;
    value_is_nullable           = false;
    value_is_container          = false;
    value_is_auto_incremented   = false;
    table                       = nullptr;
    referenced_table            = nullptr;

    type.clear();
    create_arguments.clear();

    /* conver_to_open */
    {
        std::ostringstream ss;
        for (auto it = this->attributes.begin(); it != this->attributes.end(); ++it)
        {
            switch(*it)
            {
                case attribute_t::hex:
                    ss << "HEX(";
                    break;
                case attribute_t::compress:
                    ss << "COMPRESS(";
                    break;
                case attribute_t::primary_key:
                    ss << "UuidToBin(";
                    break;
            }
        }
        convert_to_open = ss.str();
    }

    /* convert_to_close */
    {
        std::ostringstream ss;
        for (auto it = this->attributes.begin(); it != this->attributes.end(); ++it)
        {
            switch(*it)
            {
                case attribute_t::hex:
                case attribute_t::compress:
                case attribute_t::primary_key:
                    ss << ')';
                    break;
            }
        }
        convert_to_close = ss.str();
    }

    /* convert_from_open */
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
                    ss << "BinToUuid(";
                    break;
            }
        }
        convert_from_open = ss.str();
    }

    /* convert_from_close */
    {
        std::ostringstream ss;
        for (auto it = this->attributes.rbegin(); it != this->attributes.rend(); ++it)
        {
            switch(*it)
            {
                case attribute_t::hex:
                case attribute_t::compress:
                case attribute_t::primary_key:
                    ss << ')';
                    break;
            }
        }
        convert_from_close = ss.str();
    }
}

#define throw_not_implemented(p_ret, p_name, ...)               \
    p_ret field_t::p_name(__VA_ARGS__) const                    \
    {                                                           \
        throw misc::hibernate_exception(                        \
            std::string("'") + table_name + "." + field_name +  \
            "' does not implement the " #p_name "() method!");  \
    }

/* CRUD */

throw_not_implemented(value_t,          foreign_create_update, const create_update_context&)
throw_not_implemented(read_context_ptr, foreign_read,          const read_context&, const value_t&)

/* properties */

throw_not_implemented(bool,     is_default, const data_context& context)
throw_not_implemented(string,   generate_value, ::cppmariadb::connection&)
throw_not_implemented(value_t,  get, const data_context& context)
throw_not_implemented(void,     set, const data_context& context, const value_t&)