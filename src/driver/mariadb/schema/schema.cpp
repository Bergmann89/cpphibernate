#include <string>
#include <iostream>

#include <cpputils/misc/enum.h>
#include <cpputils/misc/string.h>
#include <cpputils/misc/indent.h>

#include <cpphibernate/misc.h>
#include <cpphibernate/driver/mariadb/schema/schema.h>

using namespace ::utl;
using namespace ::cpphibernate::driver::mariadb_impl;

void schema_t::update()
{
// clear everything
    for (auto& kvp : tables)
    {
        assert(static_cast<bool>(kvp.second));
        auto& table = *kvp.second;
        table.primary_key_field = nullptr;
        table.derived_tables.clear();
        table.foreign_key_fields.clear();
        table.foreign_table_fields.clear();
        table.foreign_table_one_fields.clear();
        table.foreign_table_many_fields.clear();
        table.data_fields.clear();
    }

    // update references
    for (auto& kvp : tables)
    {
        assert(static_cast<bool>(kvp.second));
        auto& table = *kvp.second;

        // base table
        auto it = tables.find(table.base_dataset_id);
        table.base_table = (it != tables.end()
            ? it->second.get()
            : nullptr);

        // derived tables
        for (auto& id : table.derived_dataset_ids)
        {
            it = tables.find(id);
            if (it == tables.end())
                throw misc::hibernate_exception(std::string("unable to find derived table for dataset id ") + std::to_string(id));
            table.derived_tables.emplace_back(it->second.get());
        }

        // update fields
        for (auto& ptr : table.fields)
        {
            auto& field = *ptr;

            // table
            if (table.dataset_id != field.table_dataset_id)
                throw misc::hibernate_exception(std::string("dataset id of field '") + field.table_name + '.' + field.field_name + "' does not match!");
            field.table = &table;

            // referenced table
            it = tables.find(field.value_dataset_id);
            auto referenced_table = (it != tables.end()
                ? it->second.get()
                : nullptr);
            field.referenced_table = referenced_table;

            // is primary key field
            if (field.attributes.count(attribute_t::primary_key))
            {
                if (static_cast<bool>(table.primary_key_field))
                    throw misc::hibernate_exception(std::string("Table '") + table.table_name + "' can not have more then one primary key!");
                table.primary_key_field = &field;
            }

            // is foreign table field
            else if (static_cast<bool>(referenced_table))
            {
                table.foreign_table_fields.emplace_back(&field);
                if (field.value_is_container)
                {
                    table.foreign_table_many_fields.emplace_back(&field);
                    referenced_table->foreign_key_fields.push_back(&field);
                }
                else
                {
                    table.foreign_table_one_fields.emplace_back(&field);
                }
            }

            // is data field
            else
            {
                table.data_fields.emplace_back(&field);
            }
        }
        if (!static_cast<bool>(table.primary_key_field))
            throw misc::hibernate_exception(std::string("Table '") + table.table_name + "' does not have a primary key!");
    }
}

void schema_t::print(std::ostream& os) const
{
    os  << indent << '{'
        << incindent
            << indent << "\"schema_name\": \""  <<  schema_name << "\","
            << indent << "\"tables\": "         <<  misc::print_container(tables, true, [](auto& os, auto& kvp) {
                                                        kvp.second->print(os);
                                                    })
        << decindent
        << indent << '}';
}