#include <string>
#include <iostream>

#include <cpputils/misc/enum.h>
#include <cpputils/misc/string.h>
#include <cpputils/misc/indent.h>

#include <cpphibernate/misc.h>
#include <cpphibernate/driver/mariadb/schema/table.h>
#include <cpphibernate/driver/mariadb/schema/filter.h>

using namespace ::utl;
using namespace ::cpphibernate::driver::mariadb_impl;

/* data_extractor_t */

struct data_extractor_t
{
    const table_t&              _table;
    const read_context&         _context;
    const ::cppmariadb::row&    _row;

    mutable size_t              _index;

    data_extractor_t(
        const table_t&              p_table,
        const read_context&         p_context,
        const ::cppmariadb::row&    p_row)
        : _table        (p_table)
        , _context      (p_context)
        , _row          (p_row)
        { }

    inline value_t get_value() const
    {
        value_t ret;
        auto f = _row.at(_index);
        if (!f.is_null())
            ret = f.get<std::string>();
        return ret;
    }

    inline void read_field(const field_t& field) const
    {
        field.set(_context, get_value());
        ++_index;
    }

    inline void read_table(const table_t& table) const
    {
        if (table.base_table)
            read_table(*table.base_table);

        if (_context.filter.is_excluded(table))
            return;

        /* primary key */
        assert(table.primary_key_field);
        read_field(*table.primary_key_field);

        /* data fields */
        for (auto& ptr : table.data_fields)
        {
            assert(ptr);
            auto& field = *ptr;
            if (!_context.filter.is_excluded(field))
                read_field(field);
        }
    }

    inline void operator()() const
    {
        _index = 0;
        _context.emplace();
        read_table(_table);
    }
};

/* select_query_builder_t */

struct select_query_builder_t
{
    const table_t&      _table;
    const filter_t&     _filter;
    bool                _is_dynamic;

    size_t              index { 0 };
    std::ostringstream  os;
    std::ostringstream  join;

    select_query_builder_t(
        const table_t&  p_table,
        const filter_t& p_filter,
        bool            p_is_dynamic)
        : _table     (p_table)
        , _filter    (p_filter)
        , _is_dynamic(p_is_dynamic)
        { }

    inline void add_field(const field_t& field)
    {
        if (index++) os << ", ";
        os  <<  "`"
            <<  field.table_name
            <<  "`.`"
            <<  field.field_name
            <<  "`";
    }

    inline bool add_table(const table_t& table, const std::string& prefix)
    {
        bool ret = false;

        if (table.base_table)
        {
            auto tmp = add_table(*table.base_table, "");
            if (tmp)
            {
                assert(table.base_table->primary_key_field);
                auto& base_key = *table.base_table->primary_key_field;
                ret = true;
                join    <<  " LEFT JOIN `"
                        <<  table.table_name
                        <<  "` ON `"
                        <<  table.table_name
                        <<  "`.`"
                        <<  base_key.field_name
                        <<  "`=`"
                        <<  base_key.table_name
                        <<  "`.`"
                        <<  base_key.field_name
                        <<  "`";
            }
        }

        /* __type */
        if (    _is_dynamic
            && !table.base_table
            && !table.derived_tables.empty())
        {
            if (index++) os << ", ";
            os  <<  "`"
                <<  table.table_name
                <<  "`.`__type` AS `__type`";
            ret = true;
        }

        if (_filter.is_excluded(table))
            return ret;

        ret = true;

        /* primary key */
        assert(table.primary_key_field);
        add_field(*table.primary_key_field);

        /* data fields */
        for (auto& ptr : table.data_fields)
        {
            assert(ptr);
            auto& field = *ptr;
            if (!_filter.is_excluded(field))
                add_field(field);
        }

        return ret;
    }

    inline std::string operator()()
    {
        os  <<  "SELECT ";
        add_table(_table, "");
        os  <<  " FROM `"
            <<  _table.table_name
            <<  "`"
            <<  join.str()
            <<  " ?where! ?order! ?limit!";
        return os.str();
    }
};

/* build queries */

std::string build_init_stage1_query(const table_t& table)
{
    std::ostringstream os;

    /* CREATE TABLE */
    os  <<  "CREATE TABLE IF NOT EXISTS `"
        <<  table.table_name
        <<  "`"
        <<  indent
        << "("
        <<  incindent;

    /* primary key */
    {
        assert(table.primary_key_field);
        auto& key_info = *table.primary_key_field;
        auto  args     = key_info.create_arguments;
        os  <<  indent
            <<  "`"
            <<  key_info.field_name
            <<  "` "
            <<  key_info.type
            <<  " NOT NULL"
            <<  (args.empty() ? "" : " ")
            <<  args
            <<  ",";
    }

    /* base table key fields */
    if (static_cast<bool>(table.base_table))
    {
        auto& base_table_info = *table.base_table;
        assert(base_table_info.primary_key_field);
        auto& key_info = *base_table_info.primary_key_field;
        os  <<  indent
            <<  "`"
            <<  key_info.field_name
            <<  "` "
            <<  key_info.type
            <<  " NOT NULL,";
    }

    /* foreign table one fields */
    for (auto& ptr : table.foreign_table_one_fields)
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
            <<  ref_key_info.type
            <<  (field_info.value_is_nullable
                    ? " NULL DEFAULT NULL,"
                    : " NOT NULL,");
    }

    /* foreign fields */
    for (auto& ptr : table.foreign_key_fields)
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
            <<  ref_key_info.type
            <<  " NULL DEFAULT NULL,";
        if (field_info.value_is_ordered)
        {
            os  <<  indent
                <<  "`"
                <<  field_info.table_name
                <<  "_index_"
                <<  field_info.field_name
                <<  "` UNSIGNED INT NOT NULL,";
        }
    }

    /* data fields */
    for (auto& ptr : table.data_fields)
    {
        assert(static_cast<bool>(ptr));
        auto&  field_info = *ptr;
        os  <<  indent
            <<  "`"
            <<  field_info.field_name
            <<  "` "
            <<  field_info.type
            <<  (field_info.value_is_nullable
                    ? " NULL DEFAULT NULL,"
                    : " NOT NULL,");
    }

    /* type field for derived tables */
    if (!table.derived_tables.empty() &&
        !table.base_table)
    {
        os  <<  indent
            <<  "`__type` INT UNSIGNED NOT NULL,";
    }

    /* PRIMARY KEY */
    {
        assert(table.primary_key_field);
        auto& key_info = *table.primary_key_field;
        os  <<  indent
            <<  "PRIMARY KEY ( `"
            <<  key_info.field_name
            <<  "` )";
    }

    /* UNIQUE INDEX primary key */
    {
        assert(table.primary_key_field);
        auto& key_info = *table.primary_key_field;
        os  <<  ','
            <<  indent
            <<  "UNIQUE INDEX `index_"
            <<  key_info.field_name
            <<  "` ( `"
            <<  key_info.field_name
            <<  "` ASC )";
    }

    /* UNIQUE INDEX base table keys */
    if (table.base_table)
    {
        auto& table_info = *table.base_table;
        assert(table_info.primary_key_field);
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
    for (auto& ptr : table.foreign_table_one_fields)
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
    for (auto& ptr : table.foreign_key_fields)
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

    /* CREATE TABLE end */
    os  <<  decindent
        <<  indent
        <<  ")"
        <<  indent
        <<  "ENGINE = InnoDB"
        <<  indent
        <<  "DEFAULT CHARACTER SET = utf8";

    return os.str();
}

std::string build_init_stage2_query(const table_t& table)
{
    std::ostringstream os;

    /* ALTER TABLE */
    os  <<  "ALTER TABLE `"
        <<  table.table_name
        <<  "`"
        <<  incindent;

    size_t index = 0;

    /* CONSTRAINT base table */
    if (table.base_table)
    {
        assert(table.base_table->primary_key_field);
        auto& ref_key_info = *table.base_table->primary_key_field;
        if (index++) os << ",";
        os  <<  indent
            <<  "ADD CONSTRAINT `fk_"
            <<  table.table_name
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
    for (auto& ptr : table.foreign_table_one_fields)
    {
        assert(static_cast<bool>(ptr));
        auto& field_info = *ptr;
        assert(field_info.referenced_table);
        assert(field_info.referenced_table->primary_key_field);
        auto& ref_key_info = *field_info.referenced_table->primary_key_field;
        if (index++) os << ",";
        os  <<  indent
            <<  "ADD CONSTRAINT `fk_"
            <<  table.table_name
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
    for (auto& ptr : table.foreign_key_fields)
    {
        assert(static_cast<bool>(ptr));
        auto&  field_info = *ptr;
        assert(field_info.table);
        assert(field_info.table->primary_key_field);
        auto&  ref_key_info = *field_info.table->primary_key_field;
        if (index++) os << ",";
        os  <<  indent
            <<  "ADD CONSTRAINT `fk_"
            <<  table.table_name
            <<  "_to_"
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

    return index == 0
        ? std::string { }
        : os.str();
}

std::string build_create_update_query(const table_t& table, const filter_t* filter, const field_t* owner)
{
    std::ostringstream os;

    size_t index     = 0;
    bool   is_update = static_cast<bool>(filter);

    /* INSER INTO / UPDATE */
    os  <<  (is_update
            ? "UPDATE"
            : "INSERT INTO")
        <<  " `"
        <<  table.table_name
        <<  "` SET ";

    /* primary key */
    if (!is_update)
    {
        assert(table.primary_key_field);
        auto& key_info = *table.primary_key_field;
        if (!key_info.value_is_auto_incremented)
        {
            if (index++)
                os << ", ";
            os  <<  "`"
                <<  key_info.field_name
                <<  "`="
                <<  key_info.convert_to_open
                <<  "?"
                <<  key_info.field_name
                <<  "?"
                <<  key_info.convert_to_close;
        }
    }

    /* base table key fields */
    if (    static_cast<bool>(table.base_table)
        && (   !is_update
            || !filter->is_excluded(*table.base_table)))
    {
        if (index++)
            os << ", ";
        auto& base_table_info = *table.base_table;
        assert(base_table_info.primary_key_field);
        auto& key_info = *base_table_info.primary_key_field;
        os  <<  "`"
            <<  key_info.field_name
            <<  "`="
            <<  key_info.convert_to_open
            <<  "?"
            <<  key_info.field_name
            <<  "?"
            <<  key_info.convert_to_close;
    }

    /* foreign table one fields */
    for (auto& ptr : table.foreign_table_one_fields)
    {
        assert(static_cast<bool>(ptr));
        auto& field_info = *ptr;
        if (is_update && filter->is_excluded(field_info))
            continue;
        if (index++)
            os << ", ";
        assert(field_info.referenced_table);
        assert(field_info.referenced_table->primary_key_field);
        auto&  key_info = *field_info.referenced_table->primary_key_field;
        os  <<  "`"
            <<  key_info.table_name
            <<  "_id_"
            <<  field_info.field_name
            <<  "`="
            <<  key_info.convert_to_open
            <<  "?"
            <<  key_info.table_name
            <<  "_id_"
            <<  field_info.field_name
            <<  "?"
            <<  key_info.convert_to_close;
    }

    /* foreign fields */
    for (auto& ptr : table.foreign_key_fields)
    {
        assert(static_cast<bool>(ptr));
        if (is_update && ptr != owner)
            continue;
        if (index++)
            os << ", ";
        auto&  field_info = *ptr;
        assert(field_info.table);
        assert(field_info.table->primary_key_field);
        auto&  key_info = *field_info.table->primary_key_field;
        os  <<  "`"
            <<  field_info.table_name
            <<  "_id_"
            <<  field_info.field_name
            <<  "`="
            <<  key_info.convert_to_open
            <<  "?"
            <<  field_info.table_name
            <<  "_id_"
            <<  field_info.field_name
            <<  "?"
            <<  key_info.convert_to_close;
        if (field_info.value_is_ordered)
        {
            if (index++)
            os << ", ";
            os  <<  "`"
                <<  field_info.table_name
                <<  "_index_"
                <<  field_info.field_name
                <<  "`=?\?";
        }
    }

    /* data fields */
    for (auto& ptr : table.data_fields)
    {
        assert(ptr);
        auto& field_info = *ptr;
        if (is_update && filter->is_excluded(field_info))
            continue;
        if (index++)
            os << ", ";
        os  <<  "`"
            <<  field_info.field_name
            <<  "`="
            <<  field_info.convert_to_open
            <<  "?"
            <<  field_info.field_name
            <<  "?"
            <<  field_info.convert_to_close;
    }

    /* type field for derived tables */
    if (!table.derived_tables.empty() &&
        !table.base_table)
    {
        if (index++)
            os << ", ";
        os  <<  "`__type`=?__type?";
    }

    /* where primary key (for update) */
    if (is_update)
    {
        assert(table.primary_key_field);
        auto& key_info = *table.primary_key_field;
        os  <<  " WHERE `"
            <<  key_info.field_name
            <<  "`="
            <<  key_info.convert_to_open
            <<  "?"
            <<  key_info.field_name
            <<  "?"
            <<  key_info.convert_to_close;
    }

    return os.str();
}

std::string build_select_query(
    const table_t& table,
    const filter_t& filter,
    bool is_dynamic)
{
    return select_query_builder_t(table, filter, is_dynamic)();
}

/* execute_create_update */

std::string table_t::execute_create_update(
    const create_update_context&    context,
    ::cppmariadb::statement&        statement,
    const filter_t*                 filter) const
{
    auto& connection = context.connection;

    size_t  index       = 0;
    bool    is_update   = static_cast<bool>(filter);

    std::string primary_key;
    statement.clear();

    /* primary key */
    assert(primary_key_field);
    if (   !primary_key_field->value_is_auto_incremented
        && !is_update)
    {
        primary_key = primary_key_field->generate_value(context.connection);
        statement.set(index, primary_key);
        ++index;
    }
    else
    {
        primary_key = *primary_key_field->get(context);
    }

    /* base_key */
    if (    base_table
        && (   !is_update
            || !filter->is_excluded(*base_table)))
    {
        auto new_context = context;
        if (!new_context.derived_table)
            new_context.derived_table = this;
        std::string key = create_update_base(new_context);
        statement.set(index, std::move(key));
        ++index;
    }

    if (is_update && filter->is_excluded(*this))
        return primary_key;

    /* foreign table one fields */
    for (auto& ptr : foreign_table_one_fields)
    {
        assert(ptr);
        if (is_update && filter->is_excluded(*ptr))
            continue;
        value_t key = ptr->foreign_create_update(context);
        if (key.has_value())    statement.set(index, std::move(key));
        else                    statement.set_null(index);
        ++index;
    }

    /* foreign fields */
    for (auto& ptr : foreign_key_fields)
    {
        assert(ptr);
        if (is_update && ptr != context.owner_field)
            continue;

        auto& field_info = *ptr;
        bool set_value =
                context.owner_field
            &&  ptr == context.owner_field;

        if (set_value)
        {
            assert(!context.owner_key.empty());
            statement.set(index, context.owner_key);
        }
        else
        {
            statement.set_null(index);
        }
        ++index;

        if (field_info.value_is_ordered)
        {
            if (set_value)  statement.set(index, context.index);
            else            statement.set(index, 0);
            ++index;
        }
    }

    /* data fields */
    for (auto& ptr : data_fields)
    {
        assert(ptr);
        if (is_update && filter->is_excluded(*ptr))
            continue;

        auto& field_info = *ptr;
        auto  value      = field_info.get(context);

        if (value.has_value())  statement.set(index, *value);
        else                    statement.set_null(index);
        ++index;
    }

    /* type field for derived tables */
    if (!derived_tables.empty() &&
        !base_table)
    {
        statement.set(index, context.derived_table
            ? context.derived_table->table_id
            : table_id);
        ++index;
    }

    /* where primary key (for update) */
    if (is_update)
    {
        assert(primary_key_field);
        statement.set(index, *primary_key_field->get(context));
        ++index;
    }

    /* execute */
    if (!is_update)
    {
        cpphibernate_debug_log("execute INSERT query: " << statement.query(connection));
    }
    else
    {
        cpphibernate_debug_log("execute UPDATE query: " << statement.query(connection));
    }

    if (    primary_key_field->value_is_auto_incremented
        && !is_update)
    {
        auto id = connection.execute_id(statement);
        primary_key = utl::to_string(id);
    }
    else
    {
        auto count = connection.execute_rows(statement);
        cpphibernate_debug_log(count << " rows inserted/updated");
    }
    primary_key_field->set(context, primary_key);

    /* foreign table many fields */
    for (auto& ptr : foreign_table_many_fields)
    {
        assert(ptr);
        if (    is_update
            && (    filter->is_excluded(*ptr)
                ||  filter->is_excluded(*ptr->referenced_table)))
            continue;

        auto next_context = context;
        next_context.owner_field   = ptr;
        next_context.owner_key     = primary_key;
        next_context.derived_table = nullptr;
        ptr->foreign_create_update(next_context);
    }

    return primary_key;
}

/* table_t */

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

const table_t* table_t::get_derived(size_t id) const
{
    if (dataset_id == id)
        return this;
    for (auto ptr : derived_tables)
    {
        assert(ptr);
        auto ret = ptr->get_derived(id);
        if (ret) return ret;
    }
    return nullptr;
}

::cppmariadb::statement& table_t::get_statement_create_table() const
{
    if (_statement_create_table)
        return *_statement_create_table;
    auto query = build_init_stage1_query(*this);
    _statement_create_table.reset(new ::cppmariadb::statement(query));
    return *_statement_create_table;
}

::cppmariadb::statement* table_t::get_statement_alter_table() const
{
    if (!_statement_alter_table)
    {
        auto query = build_init_stage2_query(*this);
        _statement_alter_table.reset(new ::cppmariadb::statement(query));
    }
    if (_statement_alter_table->empty())
        return nullptr;
    return _statement_alter_table.get();
}

::cppmariadb::statement& table_t::get_statement_insert_into() const
{
    if (_statement_insert_into)
        return *_statement_insert_into;
    auto query = build_create_update_query(*this, nullptr, nullptr);
    _statement_create_table.reset(new ::cppmariadb::statement(query));
    return *_statement_create_table;
}

::cppmariadb::statement& table_t::get_statement_select(const read_context& context) const
{
    auto& map = context.is_dynamic
        ? _statement_select_dynamic
        : _statement_select_static;
    auto it = map.find(context.filter.cache_id);
    if (it == map.end())
    {
        auto query = build_select_query(*this, context.filter, context.is_dynamic);
        it = map.emplace(context.filter.cache_id, ::cppmariadb::statement(query)).first;
    }
    return it->second;
}

std::string table_t::create_update_base(const create_update_context& context) const
{
    throw misc::hibernate_exception(static_cast<std::ostringstream&>(std::ostringstream { }
        << "'" << this->table_name << "' does not implement create_update_base!").str());
}

void table_t::init_stage1_exec(const init_context& context) const
{
    auto& statement  = get_statement_create_table();
    auto& connection = context.connection;
    cpphibernate_debug_log("execute CREATE TABLE query: " << statement.query(connection));
    connection.execute(statement);
}

void table_t::init_stage2_exec(const init_context& context) const
{
    auto* statement  = get_statement_alter_table();
    auto& connection = context.connection;
    if (!statement) return;
    cpphibernate_debug_log("execute ALTER TABLE query: " << statement->query(connection));
    connection.execute(*statement);
}

std::string table_t::create_update_exec(const create_update_context& context) const
{
    auto& statement = get_statement_insert_into();
    return execute_create_update(context, statement, nullptr);
}

std::string table_t::create_update_intern(const create_update_context& context) const
    { return create_update_exec(context); }

void table_t::read_exec(const read_context& context) const
{
    auto& statement  = get_statement_select(context);
    auto& connection = context.connection;
    cpphibernate_debug_log("execute SELECT query: " << statement.query(connection));
    auto result = connection.execute_used(statement);
    if (!result)
        throw misc::hibernate_exception("Unable to fetching data from database!");

    ::cppmariadb::row* row;
    while ((row = result->next()))
        data_extractor_t(*this, context, *row)();
    context.finish();
}