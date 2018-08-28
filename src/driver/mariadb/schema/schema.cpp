#include <string>
#include <sstream>

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

const table_t& schema_t::table(size_t dataset_id) const
{
    auto it = tables.find(dataset_id);
    if (it == tables.end())
        throw misc::hibernate_exception(std::string("unable to find table for dataset with id ") + std::to_string(dataset_id));
    assert(static_cast<bool>(it->second));
    return *it->second;
}

#define exec_query()                                                \
    do {                                                            \
        cpphibernate_debug_log("execute init query: " << ss.str()); \
        connection.execute(ss.str());                               \
        ss.str(std::string());                                      \
        ss.clear();                                                 \
    } while(0)

void schema_t::init(const init_context& context) const
{
    std::ostringstream ss;
    auto& connection = context.connection;

    if (context.recreate)
    {
        ss  <<  "DROP DATABASE IF EXISTS `"
            <<  schema_name
            <<  "`";
        exec_query();
    }

    /* create schema */
    ss  <<  "CREATE SCHEMA IF NOT EXISTS `"
        <<  schema_name
        <<  "` DEFAULT CHARACTER SET utf8";
    exec_query();

    /* use schema */
    ss  <<  "USE `"
        <<  schema_name
        <<  "`";
    exec_query();

    /* UuidToBin */
    ss  <<  "CREATE FUNCTION IF NOT EXISTS UuidToBin(_uuid CHAR(36))\n"
            "   RETURNS BINARY(16)\n"
            "   LANGUAGE SQL\n"
            "   DETERMINISTIC\n"
            "   CONTAINS SQL\n"
            "   SQL SECURITY INVOKER\n"
            "RETURN\n"
            "   UNHEX(CONCAT(\n"
            "       SUBSTR(_uuid, 25, 12),\n" // node id
            "       SUBSTR(_uuid, 20,  4),\n" // clock sequence
            "       SUBSTR(_uuid, 15,  4),\n" // time high and version
            "       SUBSTR(_uuid, 10,  4),\n" // time mid
            "       SUBSTR(_uuid,  1,  8)\n"  // time low
            "   )\n"
            ")";
    exec_query();

    /* BinToUuid */
    ss  <<  "CREATE FUNCTION IF NOT EXISTS BinToUuid(_bin BINARY(16))\n"
            "    RETURNS CHAR(36)\n"
            "    LANGUAGE SQL\n"
            "    DETERMINISTIC\n"
            "    CONTAINS SQL\n"
            "    SQL SECURITY INVOKER\n"
            "RETURN\n"
            "    LCASE(CONCAT_WS('-',\n"
            "        HEX(SUBSTR(_bin, 13, 4)),\n" // time low
            "        HEX(SUBSTR(_bin, 11, 2)),\n" // time mid
            "        HEX(SUBSTR(_bin,  9, 2)),\n" // time high and version
            "        HEX(SUBSTR(_bin,  7, 2)),\n" // clock sequence
            "        HEX(SUBSTR(_bin,  1, 6))\n"  // node id
            "   )\n"
            ")";
    exec_query();

    /* initialize tables */
    for (auto& kvp : tables)
    {
        assert(kvp.second);
        kvp.second->init(context);
    }
}

#undef exec_query