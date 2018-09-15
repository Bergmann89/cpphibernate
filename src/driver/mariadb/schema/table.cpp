#include <string>
#include <iostream>

#include <cpputils/misc/enum.h>
#include <cpputils/misc/string.h>
#include <cpputils/misc/indent.h>

#include <cpphibernate/misc.h>
#include <cpphibernate/driver/mariadb/schema/table.h>
#include <cpphibernate/driver/mariadb/schema/filter.h>

using namespace ::utl;
using namespace ::cpphibernate;
using namespace ::cpphibernate::driver::mariadb_impl;

/* data_extractor_t */

struct foreign_many_tuple_t
{
    const field_t&      field;
    read_context_ptr    context;
    std::string         owner;
};

struct foreign_many_list_t :
    public std::list<foreign_many_tuple_t>
    { };

struct data_extractor_t
{
    const table_t&              _table;
    const read_context&         _context;
    const ::cppmariadb::row&    _row;
    foreign_many_list_t&        _foreign_many_list;

    const filter_t&             _filter;
    mutable size_t              _index;

    data_extractor_t(
        const table_t&              p_table,
        const read_context&         p_context,
        const ::cppmariadb::row&    p_row,
        foreign_many_list_t&        p_foreign_many_list)
        : _table            (p_table)
        , _context          (p_context)
        , _row              (p_row)
        , _foreign_many_list(p_foreign_many_list)
        , _filter           (p_context.filter)
        , _index            (0)
        { }

    inline bool has_value() const
        { return !_row.at(_index).is_null(); }

    inline value_t get_value() const
    {
        value_t ret;
        auto f = _row.at(_index);
        if (!f.is_null())
            ret = f.get<std::string>();
        return ret;
    }

    inline void next_field() const
        { ++_index; }

    inline void read_field(const field_t& field, const read_context& context, bool skip = false) const
    {
        auto value = get_value();
        ++_index;
        if (!skip)
            field.set(context, value);
    }

    inline bool read_table(const table_t& table, const read_context& context, bool read_base, bool read_derived, bool skip = false) const
    {
        /* read the base table */
        if (read_base && table.base_table)
        {
            skip = read_table(*table.base_table, context, true, false, skip);
        }

        /* create a dynamic dataset depending on the derived table */
        else if (    read_base
                 &&  context.is_dynamic
                 && !table.derived_tables.empty())
        {
            auto value = get_value();
            next_field();
            if (static_cast<bool>(value) && !skip)
            {
                auto type = utl::from_string<uint>(*value);
                auto derived = _table.get_derived_by_table_id(type);
                if (!derived)
                    throw misc::hibernate_exception(std::string("unable to find dereived table for id ") + std::to_string(type));
                derived->emplace(context);
            }
            else
            {
                skip = true;
            }
        }

        /* create a static dataset */
        else if (has_value() && !skip)
        {
            if (read_base)
            {
                context.emplace();
            }
        }

        /* no data -> skip */
        else
        {
            skip = true;
        }

        if (_context.filter.is_excluded(table))
            return skip;

        /* primary key */
        assert(table.primary_key_field);
        read_field(*table.primary_key_field, context, skip);

        /* data fields */
        for (auto& ptr : table.data_fields)
        {
            assert(ptr);
            auto& field = *ptr;
            if (!_context.filter.is_excluded(field))
                read_field(field, context, skip);
        }

        /* foreign table one */
        for (auto& ptr : table.foreign_table_one_fields)
        {
            assert(ptr);
            assert(ptr->referenced_table);

            auto& field     = *ptr;
            auto& ref_table = *field.referenced_table;

            if (    _filter.is_excluded(field)
                ||  _filter.is_excluded(ref_table))
                continue;

            auto next_context = field.foreign_read(context, skip);
            assert(static_cast<bool>(next_context));
            read_table(ref_table, *next_context, true, true, skip);
            next_context->finish();
        }

        /* foreign table many */
        if (!skip)
        {
            for (auto& ptr : table.foreign_table_many_fields)
            {
                assert(ptr);
                assert(ptr->referenced_table);

                auto& field     = *ptr;
                auto& ref_table = *field.referenced_table;

                if (    _filter.is_excluded(field)
                    ||  _filter.is_excluded(ref_table))
                    continue;

                _foreign_many_list.emplace_back(
                    foreign_many_tuple_t {
                        field,
                        field.foreign_read(context, false),
                        *table.primary_key_field->get(context),
                    });
            }
        }

        /* derived tables */
        if (read_derived && context.is_dynamic)
        {
            for (auto& ptr : table.derived_tables)
            {
                assert(ptr);
                auto& derived_table = *ptr;
                read_table(derived_table, context, false, true, skip);
            }
        }

        return skip;
    }

    inline void operator()() const
    {
        _index = 0;
        read_table(_table, _context, true, true, false);
        if (_index != _row.size())
            throw misc::hibernate_exception("result was not completely read!");
    }
};

/* select_query_builder_t */

struct select_query_builder_t
{
    struct local_context
    {
        const table_t&      table;
        std::string         alias;
        bool                add_base;
        bool                add_derived;
        bool                is_dynamic;
    };

    const table_t&          _table;
    const filter_t&         _filter;
    bool                    _is_dynamic;

    size_t                  alias_id { 0 };
    size_t                  index    { 0 };
    std::ostringstream      os;
    std::list<std::string>  joins;

    select_query_builder_t(
        const table_t&  p_table,
        const filter_t& p_filter,
        bool            p_is_dynamic)
        : _table     (p_table)
        , _filter    (p_filter)
        , _is_dynamic(p_is_dynamic)
        { }

    inline std::string make_alias()
        { return std::string("T") + std::to_string(alias_id++); }

    inline void add_field(const field_t& field, const std::string& alias)
    {
        if (index++) os << ", ";
        os  <<  field.convert_from_open
            <<  "`"
            <<  alias
            <<  "`.`"
            <<  field.field_name
            <<  "`"
            <<  field.convert_from_close;
    }

    inline bool add_table(const local_context& ctx)
    {
        bool ret        = false;
        auto has_alias  = !ctx.alias.empty();
        auto real_alias = has_alias ? ctx.alias : ctx.table.table_name;

        if (ctx.table.base_table && ctx.add_base)
        {
            assert(ctx.table.base_table->primary_key_field);
            auto& base_table      = *ctx.table.base_table;
            auto& base_key        = *base_table.primary_key_field;
            auto  base_alias      = has_alias ? make_alias() : std::string();
            auto  real_base_alias = has_alias ? base_alias : base_table.table_name;

            std::ostringstream ss;
            ss  <<  " JOIN `"
                <<  base_table.table_name;
            if (has_alias)
            {
                ss  <<  "` AS `"
                    <<  base_alias;
            }
            ss  <<  "` ON `"
                <<  real_alias
                <<  "`.`"
                <<  base_key.field_name
                <<  "`=`"
                <<  real_base_alias
                <<  "`.`"
                <<  base_key.field_name
                <<  "`";

            auto it = joins.insert(joins.end(), ss.str());
            if (add_table({
                    base_table,
                    base_alias,
                    true,
                    false,
                    ctx.is_dynamic
                }))
            {
                ret = true;
            }
            else
            {
                joins.erase(it);
            }
        }

        /* __type */
        if (    ctx.is_dynamic
            && !ctx.table.base_table
            && !ctx.table.derived_tables.empty())
        {
            if (index++) os << ", ";
            os  <<  "`"
                <<  ctx.table.table_name
                <<  "`.`__type` AS `__type`";
            ret = true;
        }

        if (_filter.is_excluded(ctx.table))
            return ret;

        ret = true;

        /* primary key */
        assert(ctx.table.primary_key_field);
        add_field(*ctx.table.primary_key_field, real_alias);

        /* data fields */
        for (auto& ptr : ctx.table.data_fields)
        {
            assert(ptr);
            auto& field = *ptr;
            if (!_filter.is_excluded(field))
                add_field(field, real_alias);
        }

        /* foreign table one */
        for (auto& ptr : ctx.table.foreign_table_one_fields)
        {
            assert(ptr);
            assert(ptr->referenced_table);
            assert(ptr->referenced_table->primary_key_field);

            auto& field     = *ptr;
            auto& ref_table = *field.referenced_table;
            auto& ref_key   = *ref_table.primary_key_field;

            if (    _filter.is_excluded(field)
                ||  _filter.is_excluded(ref_table))
                continue;

            auto new_alias = make_alias();

            std::ostringstream ss;
            ss  <<  " LEFT JOIN `"
                <<  ref_table.table_name
                <<  "` AS `"
                <<  new_alias
                <<  "` ON `"
                <<  real_alias
                <<  "`.`"
                <<  ref_key.table_name
                <<  "_id_"
                <<  field.field_name
                <<  "`=`"
                <<  new_alias
                <<  "`.`"
                <<  ref_key.field_name
                <<  "`";

            auto it = joins.insert(joins.end(), ss.str());
            if (!add_table({
                    ref_table,
                    new_alias,
                    true,
                    true,
                    field.value_is_pointer
                }))
            {
                joins.erase(it);
            }
        }

        /* derived tables */
        if (ctx.add_derived && ctx.is_dynamic)
        {
            for (auto& ptr : ctx.table.derived_tables)
            {
                assert(ptr);
                assert(ptr->primary_key_field);
                auto& derived_table      = *ptr;
                auto& primary_key        = *ctx.table.primary_key_field;
                auto  derived_alias      = has_alias ? make_alias() : std::string();
                auto  real_derived_alias = has_alias ? derived_alias : derived_table.table_name;

                std::ostringstream ss;
                ss  <<  " LEFT JOIN `"
                    <<  derived_table.table_name;
                if (has_alias)
                {
                    ss  <<  "` AS `"
                        <<  derived_alias;
                }
                ss  <<  "` ON `"
                    <<  real_alias
                    <<  "`.`"
                    <<  primary_key.field_name
                    <<  "`=`"
                    <<  real_derived_alias
                    <<  "`.`"
                    <<  primary_key.field_name
                    <<  "`";

                auto it = joins.insert(joins.end(), ss.str());
                if (add_table({
                        derived_table,
                        derived_alias,
                        false,
                        true,
                        ctx.is_dynamic,
                    }))
                {
                    ret = true;
                }
                else
                {
                    joins.erase(it);
                }
            }
        }

        return ret;
    }

    inline std::string operator()()
    {
        os  <<  "SELECT ";
        add_table({
            _table,
            "",
            true,
            true,
            _is_dynamic,
        });
        os  <<  " FROM `"
            <<  _table.table_name
            <<  "`";

        for (auto& join : joins)
            os << join;

        os  <<  " ?where! ?order! ?limit!";
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
                <<  "` INT UNSIGNED NOT NULL,";
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
    if (   !table.derived_tables.empty()
        && !table.base_table
        && !is_update)
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
    ::cppmariadb::statement&        statement) const
{
    auto& connection = context.connection;
    auto& filter     = context.filter;

    size_t  index       = 0;
    bool    is_update   = context.is_update;

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
            || !filter.is_excluded(*base_table)))
    {
        auto new_context = context;
        if (!new_context.derived_table)
            new_context.derived_table = this;
        std::string key = create_update_base(new_context);
        statement.set(index, std::move(key));
        ++index;
    }

    if (is_update && filter.is_excluded(*this))
        return primary_key;

    /* foreign table one fields */
    for (auto& ptr : foreign_table_one_fields)
    {
        assert(ptr);
        auto& field = *ptr;
        if (is_update && filter.is_excluded(field))
            continue;

        /* insert/update dataset */
        value_t key = field.foreign_create_update(context);
        if (key.has_value())
            statement.set(index, std::move(key));
        else if (field.value_is_nullable)
            statement.set_null(index);
        else
            throw misc::hibernate_exception("Received null key for non nullable foreign dataset!");
        ++index;

        /* cleanup old dataset (if new one was created) */
        if (context.is_update)
        {
            auto& delete_statement = field.get_statement_foreign_one_delete(key.has_value());
            delete_statement.set(0, primary_key);
            if (key.has_value())
                delete_statement.set(1, *key);
            cpphibernate_debug_log("execute DELETE old foreign one query: " << delete_statement.query(connection));
            connection.execute(delete_statement);
        }
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
        if (is_update && filter.is_excluded(*ptr))
            continue;

        auto& field_info = *ptr;
        auto  value      = field_info.get(context);

        if (value.has_value())  statement.set(index, *value);
        else                    statement.set_null(index);
        ++index;
    }

    /* type field for derived tables */
    if (   !derived_tables.empty()
        && !base_table
        && !is_update)
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
        if (count != 1)
            throw misc::hibernate_exception("Expected exaclty one row to be inserted/updated!");
        cpphibernate_debug_log(count << " rows inserted/updated");
    }
    primary_key_field->set(context, primary_key);

    /* foreign table many fields */
    for (auto& ptr : foreign_table_many_fields)
    {
        assert(ptr);
        assert(ptr->referenced_table);

        auto& field     = *ptr;
        auto& ref_table = *field.referenced_table;

        if (    is_update
            && (    filter.is_excluded(field)
                ||  filter.is_excluded(ref_table)))
            continue;

        /* set foreign keys of existing elements to null */
        if (context.is_update)
        {
            auto& update_statement = field.get_statement_foreign_many_update();
            update_statement.set(0, primary_key);
            cpphibernate_debug_log("execute UPDATE old foreign many query: " << update_statement.query(connection));
            connection.execute(update_statement);
        }

        /* update elements */
        auto next_context = context;
        next_context.owner_field   = ptr;
        next_context.owner_key     = primary_key;
        next_context.derived_table = nullptr;
        field.foreign_create_update(next_context);

        /* delete non referenced elements */
        if (context.is_update)
        {
            auto& delete_statement = ref_table.get_statement_foreign_many_delete();
            cpphibernate_debug_log("execute DELETE old foreign many query: " << delete_statement.query(connection));
            connection.execute(delete_statement);
        }
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

const table_t* table_t::get_derived_by_table_id(size_t id) const
{
    if (table_id == id)
        return this;
    for (auto ptr : derived_tables)
    {
        assert(ptr);
        auto ret = ptr->get_derived_by_table_id(id);
        if (ret) return ret;
    }
    return nullptr;
}

const table_t* table_t::get_derived_by_dataset_id(size_t id) const
{
    if (dataset_id == id)
        return this;
    for (auto ptr : derived_tables)
    {
        assert(ptr);
        auto ret = ptr->get_derived_by_dataset_id(id);
        if (ret) return ret;
    }
    return nullptr;
}

void table_t::emplace(const read_context& context) const
    { throw misc::hibernate_exception(std::string("'") + table_name + "' does not implement the emplace() method!"); }

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
    auto key = std::make_tuple(context.filter.cache_id, static_cast<const field_t*>(nullptr));
    auto it  = map.find(key);
    if (it == map.end())
    {
        auto query = build_select_query(*this, context.filter, context.is_dynamic);
             it    = map.emplace(key, ::cppmariadb::statement(query)).first;
    }
    return it->second;
}

::cppmariadb::statement& table_t::get_statement_update(const filter_t& filter, const field_t* owner) const
{
    auto key = std::make_tuple(filter.cache_id, owner);
    auto it  = _statement_update.find(key);
    if (it == _statement_update.end())
    {
        auto query = build_create_update_query(*this, &filter, owner);
             it    = _statement_update.emplace(key, ::cppmariadb::statement(query)).first;
    }
    return it->second;
}

::cppmariadb::statement& table_t::get_statement_foreign_many_delete() const
{
    if (!_statement_foreign_many_delete)
    {
        std::ostringstream os;
        os  <<  "DELETE FROM `"
            <<  table_name
            <<  "` WHERE";
        auto first = true;
        for (auto ptr : foreign_key_fields)
        {
            assert(ptr);
            auto& field = *ptr;
            if (first)
                first = false;
            else
                os << " AND";
            os  <<  " (`"
                <<  field.table_name
                <<  "_id_"
                <<  field.field_name
                <<  "` IS NULL)";
        }
        _statement_foreign_many_delete.reset(new ::cppmariadb::statement(os.str()));
    }
    return *_statement_foreign_many_delete;
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
    auto& statement = context.is_update
        ? get_statement_update(context.filter, context.owner_field)
        : get_statement_insert_into();
    return execute_create_update(context, statement);
}

std::string table_t::create_update_intern(const create_update_context& context) const
    { return create_update_exec(context); }

void table_t::read_exec(const read_context& context) const
{
    auto& statement  = get_statement_select(context);
    auto& connection = context.connection;

    statement.set(0, context.where);
    statement.set(1, context.order_by);
    statement.set(2, context.limit);

    cpphibernate_debug_log("execute SELECT query: " << statement.query(connection));
    auto result = connection.execute_used(statement);
    if (!result)
        throw misc::hibernate_exception("Unable to fetching data from database!");

    ::cppmariadb::row* row;
    foreign_many_list_t foreign_many_list;
    while ((row = result->next()))
        data_extractor_t(*this, context, *row, foreign_many_list)();
    context.finish();

    for (auto& tuple : foreign_many_list)
    {
        auto& field        = tuple.field;
        auto& next_context = *tuple.context;
        assert(field.table);
        assert(field.referenced_table);
        assert(field.referenced_table->primary_key_field);
        auto& ref_table    = *field.referenced_table;
        auto& ref_field    = *ref_table.primary_key_field;

        {
        std::ostringstream ss;
        ss  <<  "WHERE (`"
            <<  ref_table.table_name
            <<  "`.`"
            <<  field.table_name
            <<  "_id_"
            <<  field.field_name
            <<  "`="
            <<  ref_field.convert_to_open
            <<  "'"
            <<  context.connection.escape(tuple.owner)
            <<  "'"
            <<  ref_field.convert_to_close
            <<  ")";
        next_context.where = ss.str();
        }

        {
        std::ostringstream ss;
        ss  <<  "ORDER BY `"
            <<  ref_table.table_name
            <<  "`.`"
            <<  field.table_name
            <<  "_index_"
            <<  field.field_name
            <<  "` ASC";
        next_context.order_by = ss.str();
        }

        ref_table.read(next_context);
    }
}