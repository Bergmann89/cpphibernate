#pragma once

#include <sstream>
#include <cpphibernate/config.h>
#include <cpphibernate/modifier.h>
#include <cpphibernate/driver/mariadb/schema/schema.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* where_builder */

    template<typename T_modifiers>
    struct where_builder
    {
    private:
        struct build_t
        {
            const schema_t&     schema;
            const T_modifiers&  modifiers;
            std::ostringstream  os;

            inline build_t(const schema_t& p_schema, const T_modifiers& p_modifiers)
                : schema    (p_schema)
                , modifiers (p_modifiers)
                { }

            template<typename T_clause>
            inline auto build_clause(T_clause&& clause)
                -> mp::enable_if<modifier::is_where_clause_and<mp::decay_t<T_clause>>>
            {
                os << "(";
                build_clause(os, clause.clauses[hana::size_c<0>]);
                os << ")";
                hana::for_each(
                    hana::remove_at(clause.clauses, hana::size_c<0>),
                    [&](auto& clause) {
                        os << " AND (";
                        build_clause(os, clause);
                        os << ")";
                    });
            }

            template<typename T_clause>
            inline auto build_clause(T_clause&& clause)
                -> mp::enable_if<modifier::is_where_clause_or<mp::decay_t<T_clause>>>
            {
                os << "(";
                build_clause(os, clause.clauses[hana::size_c<0>]);
                os << ")";
                hana::for_each(
                    hana::remove_at(clause.clauses, hana::size_c<0>),
                    [&](auto& clause) {
                        os << " OR (";
                        build_clause(os, clause);
                        os << ")";
                    });
            }

            template<typename T_clause>
            inline auto build_clause(T_clause&& clause)
                -> mp::enable_if<modifier::is_where_clause_not<mp::decay_t<T_clause>>>
            {
                os << "NOT (";
                build_clause(os, clause.clause);
                os << ")";
            }

            template<typename T_clause>
            inline auto build_clause(T_clause&& clause)
                -> mp::enable_if<modifier::is_where_clause_equal<mp::decay_t<T_clause>>>
            {
                auto field_id = misc::get_type_id(hana::type_c<mp::decay_t<decltype(clause.field)>>);
                auto& field = schema.field(field_id);
                os  <<  "`"
                    <<  field.table_name
                    <<  "`.`"
                    <<  field.field_name
                    <<  "`="
                    <<  field.convert_to_open
                    <<  "?\?"
                    <<  field.convert_to_close;
            }

            inline void build(::cppmariadb::statement& statement)
            {
                size_t index = 0;
                hana::for_each(modifiers, [&](auto& modifier){
                    using modifier_type = mp::decay_t<decltype(modifier)>;
                    using is_where_type = modifier::is_where<modifier_type>;
                    hana::eval_if(
                        is_where_type { },
                        [&](auto _){
                            if (index++ == 0)   os << "WHERE (";
                            else                os << " AND (";
                            build_clause(_(modifier).clause);
                            os << ")";
                        },
                        []{ });
                });
                statement.assign(os.str());
            }
        };

        struct assign_t
        {
            ::cppmariadb::statement&    statement;
            const T_modifiers&          modifiers;
            size_t                      index { 0 };

            inline assign_t(::cppmariadb::statement& p_statement, const T_modifiers& p_modifiers)
                : statement(p_statement)
                , modifiers(p_modifiers)
                { }

            template<typename T_clause>
            inline auto assign_clause(T_clause&& clause)
                -> mp::enable_if_c<
                        modifier::is_where_clause_and<mp::decay_t<T_clause>>::value
                    ||  modifier::is_where_clause_or <mp::decay_t<T_clause>>::value>
            {
                hana::for_each([&](auto& clause) {
                    assign_clause(clause);
                });
            }

            template<typename T_clause>
            inline auto assign_clause(T_clause&& clause)
                -> mp::enable_if<modifier::is_where_clause_not<mp::decay_t<T_clause>>>
            {
                assign_clause(clause.clause);
            }

            template<typename T_clause>
            inline auto assign_clause(T_clause&& clause)
                -> mp::enable_if<modifier::is_where_clause_equal<mp::decay_t<T_clause>>>
            {
                statement.set(index, clause.value);
                ++index;
            }

            inline void assign()
            {
                hana::for_each(modifiers, [&](auto& modifier){
                    using modifier_type = mp::decay_t<decltype(modifier)>;
                    using is_where_type = modifier::is_where<modifier_type>;
                    hana::eval_if(
                        is_where_type { },
                        [&](auto _){
                            assign_clause(_(modifier).clause);
                        },
                        []{ });
                });
            }
        };

    private:
        const schema_t*         _schema     { nullptr };
        ::cppmariadb::statement _statement;

    public:
        inline ::cppmariadb::statement& assign(const schema_t& schema, const T_modifiers& modifiers)
        {
            if (_schema != &schema)
            {
                build_t(schema, modifiers).build(_statement);
                _schema = &schema;
            }
            assign_t(_statement, modifiers).assign();
            return _statement;
        }
    };

    template<typename T_modifiers>
    inline decltype(auto) build_where(const schema_t& schema, const T_modifiers& modifiers)
    {
        static where_builder<T_modifiers> builder;
        return builder.assign(schema, modifiers);
    }

}
end_namespace_cpphibernate_driver_mariadb
