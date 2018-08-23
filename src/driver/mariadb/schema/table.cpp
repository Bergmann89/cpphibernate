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

::cppmariadb::statement& table_t::get_statement_create_table() const
{
    if (_statement_create_table)
        return *_statement_create_table;

    std::ostringstream os;

    /* CREATE TABLE */
    os  <<  "CREATE TABLE IF NOT EXISTS `"
        <<  table_name
        <<  "`"
        <<  indent
        << "("
        <<  incindent;

    /* primary key */
    {
        assert(primary_key_field);
        auto& key_info = *primary_key_field;
        auto  args     = key_info.create_table_arguments();
        os  <<  indent
            <<  "`"
            <<  key_info.field_name
            <<  "` "
            <<  key_info.type()
            <<  " NOT NULL"
            <<  (args.empty() ? "" : " ")
            <<  args
            <<  ",";
    }

    /* base table key fields */
    if (static_cast<bool>(base_table))
    {
        auto& base_table_info = *base_table;
        assert(base_table_info.primary_key_field);
        auto& key_info = *base_table_info.primary_key_field;
        os  <<  indent
            <<  "`"
            <<  key_info.field_name
            <<  "` "
            <<  key_info.type()
            <<  " NOT NULL,";
    }

    /* foreign table one fields */
    for (auto& ptr : foreign_table_one_fields)
    {
        assert(static_cast<bool>(ptr));
        auto&  field_info = *ptr;
        assert(field_info.referenced_table);
        assert(field_info.referenced_table->primary_key_field);
        auto& ref_key_info = *field_info.referenced_table->primary_key_field;
        os  <<  indent
            <<  "`"
            <<  ref_key_info.table_name
            <<  "_id_"
            <<  field_info.field_name
            <<  "` "
            <<  ref_key_info.type()
            <<  (field_info.value_is_nullable
                    ? " NULL DEFAULT NULL,"
                    : " NOT NULL,");
    }

    /* foreign fields */
    for (auto& ptr : foreign_key_fields)
    {
        assert(static_cast<bool>(ptr));
        auto&  field_info = *ptr;
        assert(field_info.table);
        assert(field_info.table->primary_key_field);
        auto&  ref_key_info = *field_info.table->primary_key_field;
        os  <<  indent
            <<  "`"
            <<  field_info.table_name
            <<  "_id_"
            <<  field_info.field_name
            <<  "` "
            <<  ref_key_info.type()
            <<  " NULL DEFAULT NULL,";
    }

    /* data fields */
    for (auto& ptr : data_fields)
    {
        assert(static_cast<bool>(ptr));
        auto&  field_info = *ptr;
        os  <<  indent
            <<  "`"
            <<  field_info.field_name
            <<  "` "
            <<  field_info.type()
            <<  (field_info.value_is_nullable
                    ? " NULL DEFAULT NULL,"
                    : " NOT NULL,");
    }

    /* type field for derived tables */
    if (!derived_tables.empty() &&
        !base_table)
    {
        os  <<  indent
            <<  "`__type` INT UNSIGNED NOT NULL,";
    }

    /* PRIMARY KEY */
    {
        os  <<  indent
            <<  "PRIMARY KEY ( `"
            <<  primary_key_field->field_name
            <<  "` )";
    }

    /* UNIQUE INDEX primary key */
    os  <<  ','
        <<  indent
        <<  "UNIQUE INDEX `index_"
        <<  primary_key_field->field_name
        <<  "` ( `"
        <<  primary_key_field->field_name
        <<  "` ASC )";

    /* UNIQUE INDEX base table keys */
    if (base_table)
    {
        auto& table_info = *base_table;
        auto& key_info   = *table_info.primary_key_field;
        os  <<  ','
            <<  indent
            <<  "UNIQUE INDEX `index_"
            <<  key_info.field_name
            <<  "` ( `"
            <<  key_info.field_name
            <<  "` ASC )";
    }

    /* INDEX foreign table one fields */
    for (auto& ptr : foreign_table_one_fields)
    {
        assert(static_cast<bool>(ptr));
        auto& field_info = *ptr;
        assert(field_info.referenced_table);
        assert(field_info.referenced_table->primary_key_field);
        auto& ref_key_info = *field_info.referenced_table->primary_key_field;
        os  <<  ","
            <<  indent
            <<  "INDEX `index_"
            <<  ref_key_info.table_name
            <<  "_id_"
            <<  field_info.field_name
            <<  "` ( `"
            <<  ref_key_info.table_name
            <<  "_id_"
            <<  field_info.field_name
            <<  "` ASC )";
    }

    /* INDEX foreign fields */
    for (auto& ptr : foreign_key_fields)
    {
        assert(static_cast<bool>(ptr));
        auto& field_info = *ptr;
        os  <<  ","
            <<  indent
            <<  "INDEX `index_"
            <<  field_info.table_name
            <<  "_id_"
            <<  field_info.field_name
            <<  "` ( `"
            <<  field_info.table_name
            <<  "_id_"
            <<  field_info.field_name
            <<  "` ASC )";
    }

    /* CONSTRAINT base table */
    if (base_table)
    {
        assert(base_table->primary_key_field);
        auto& ref_key_info = *base_table->primary_key_field;
        os  <<  ","
            <<  indent
            <<  "CONSTRAINT `fk_"
            <<  table_name
            <<  "_to_"
            <<  ref_key_info.field_name
            <<  "`"
            <<  incindent
            <<  indent
            <<  "FOREIGN KEY (`"
            <<  ref_key_info.field_name
            <<  "`)"
            <<  indent
            <<  "REFERENCES `"
            <<  ref_key_info.schema_name
            <<  "`.`"
            <<  ref_key_info.table_name
            <<  "` (`"
            <<  ref_key_info.field_name
            <<  "`)"
            <<  indent
            <<  "ON DELETE CASCADE"
            <<  indent
            <<  "ON UPDATE NO ACTION"
            <<  decindent;
    }

    /* CONSTRAINT foreign table one fields */
    for (auto& ptr : foreign_table_one_fields)
    {
        assert(static_cast<bool>(ptr));
        auto& field_info = *ptr;
        assert(field_info.referenced_table);
        assert(field_info.referenced_table->primary_key_field);
        auto& ref_key_info = *field_info.referenced_table->primary_key_field;
        os  <<  ","
            <<  indent
            <<  "CONSTRAINT `fk_"
            <<  table_name
            <<  "_to_"
            <<  ref_key_info.table_name
            <<  "_id_"
            <<  field_info.field_name
            <<  "`"
            <<  incindent
            <<  indent
            <<  "FOREIGN KEY (`"
            <<  ref_key_info.table_name
            <<  "_id_"
            <<  field_info.field_name
            <<  "`)"
            <<  indent
            <<  "REFERENCES `"
            <<  ref_key_info.schema_name
            <<  "`.`"
            <<  ref_key_info.table_name
            <<  "` (`"
            <<  ref_key_info.field_name
            <<  "`)"
            <<  indent
            <<  "ON DELETE CASCADE"
            <<  indent
            <<  "ON UPDATE NO ACTION"
            <<  decindent;
    }

    /* CONSTRAINT foreign fields */
    for (auto& ptr : foreign_key_fields)
    {
        assert(static_cast<bool>(ptr));
        auto&  field_info = *ptr;
        assert(field_info.table);
        assert(field_info.table->primary_key_field);
        auto&  ref_key_info = *field_info.table->primary_key_field;
        os  <<  ","
            <<  indent
            <<  "CONSTRAINT `fk_"
            <<  table_name
            <<  "_"
            <<  field_info.table_name
            <<  "_id_"
            <<  field_info.field_name
            <<  "`"
            <<  incindent
            <<  indent
            <<  "FOREIGN KEY (`"
            <<  field_info.table_name
            <<  "_id_"
            <<  field_info.field_name
            <<  "`)"
            <<  indent
            <<  "REFERENCES `"
            <<  ref_key_info.schema_name
            <<  "`.`"
            <<  ref_key_info.table_name
            <<  "` (`"
            <<  ref_key_info.field_name
            <<  "`)"
            <<  indent
            <<  "ON DELETE SET NULL"
            <<  indent
            <<  "ON UPDATE NO ACTION"
            <<  decindent;
    }

    /* CREATE TABLE end */
    os  <<  decindent
        <<  indent
        <<  ")"
        <<  indent
        <<  "ENGINE = InnoDB"
        <<  indent
        <<  "DEFAULT CHARACTER SET = utf8";

    _statement_create_table.reset(new ::cppmariadb::statement(os.str()));
    return *_statement_create_table;
}

void table_t::init_intern(const init_context& context) const
{
    auto& statement  = get_statement_create_table();
    auto& connection = context.connection;
    cpphibernate_debug_log("execute init query: " << statement.query(connection));
    connection.execute(statement);
}