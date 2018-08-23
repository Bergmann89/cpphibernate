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

#define throw_not_implemented(p_ret, p_name)                    \
    p_ret field_t::p_name() const                               \
    {                                                           \
        throw misc::hibernate_exception(                        \
            std::string("'") + table_name + "." + field_name +  \
            "' does not implement the " #p_name "() method!");  \
    }

throw_not_implemented(string, type)
throw_not_implemented(string, create_table_arguments)
