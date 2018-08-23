#include <string>
#include <iostream>

#include <cpputils/misc/enum.h>
#include <cpputils/misc/string.h>
#include <cpputils/misc/indent.h>

#include <cpphibernate/misc.h>
#include <cpphibernate/driver/mariadb/schema/table.h>

using namespace ::utl;
using namespace ::cpphibernate::driver::mariadb_impl;

void table_t::print(std::ostream& os) const
{
    os  << indent << '{'
        << incindent
            << indent << "\"dataset_id\": "                 <<  dataset_id << ","
            << indent << "\"base_dataset_id\": "            <<  base_dataset_id << ","
            << indent << "\"table_id\": "                   <<  table_id << ","
            << indent << "\"derived_dataset_ids\": "        <<  misc::print_container(derived_dataset_ids, false) << ","
            << indent << "\"schema_name\": \""              <<  schema_name << "\","
            << indent << "\"table_name\": \""               <<  table_name << "\","
            << indent << "\"fields\":"                      <<  misc::print_container(fields, true, [](auto& os, auto& field) {
                                                                    field->print(os);
                                                                }) << ","
            << indent << "\"base_table\": "                 <<  (base_table ? std::string("\"") + base_table->table_name + "\"" : "null") << ","
            << indent << "\"derived_tables\":"              <<  misc::print_container(derived_tables, true, [](auto& os, auto& ptr){
                                                                    os << indent << '"' << ptr->table_name << '"';
                                                                }) << ","
            << indent << "\"primary_key_field\": "          <<  (primary_key_field ? std::string("\"") + primary_key_field->field_name + "\"" : "null") << ","
            << indent << "\"foreign_key_fields\": "         <<  misc::print_container(foreign_key_fields, true, [](auto& os, auto& ptr){
                                                                    os << indent << '"' << ptr->table_name << '.' << ptr->field_name << '"';
                                                                }) << ","
            << indent << "\"foreign_table_fields\": "       <<  misc::print_container(foreign_table_fields, true, [](auto& os, auto& ptr){
                                                                    os << indent << '"' << ptr->field_name << '"';
                                                                }) << ","
            << indent << "\"foreign_table_one_fields\": "   <<  misc::print_container(foreign_table_one_fields, true, [](auto& os, auto& ptr){
                                                                    os << indent << '"' << ptr->field_name << '"';
                                                                }) << ","
            << indent << "\"foreign_table_many_fields\": "  <<  misc::print_container(foreign_table_many_fields, true, [](auto& os, auto& ptr){
                                                                    os << indent << '"' << ptr->field_name << '"';
                                                                }) << ","
            << indent << "\"data_fields\": "                <<  misc::print_container(data_fields, true, [](auto& os, auto& ptr){
                                                                    os << indent << '"' << ptr->field_name << '"';
                                                                })
        << decindent
        << indent << '}';
}