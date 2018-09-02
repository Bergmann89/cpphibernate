#pragma once

#include <sstream>
#include <cpphibernate/config.h>
#include <cpphibernate/modifier.h>
#include <cpphibernate/driver/mariadb/schema/schema.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* order_by_builder */

    template<typename T_modifiers>
    struct order_by_builder
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

            inline void build(::cppmariadb::statement& statement)
            {
                size_t index = 0;
                hana::for_each(modifiers, [&](auto& modifier){
                    using modifier_type    = mp::decay_t<decltype(modifier)>;
                    using is_order_by_type = modifier::is_order_by<modifier_type>;
                    hana::eval_if(
                        is_order_by_type { },
                        [&](auto _){
                            hana::for_each(_(modifier).fields, [&](auto& f){
                                using field_type        = mp::decay_t<decltype(f)>;
                                using is_ascencing_type = modifier::is_ascending<field_type>;
                                if (index++ == 0)   os << "ORDER_BY ";
                                else                os << ", ";
                                auto& field = schema.field(misc::get_type_id(f.wrapped_field));
                                os  <<  "`"
                                    <<  field.table_name
                                    <<  "`.`"
                                    <<  field.field_name
                                    <<  "` "
                                    <<  (is_ascencing_type::value
                                            ? "ASC"
                                            : "DESC");
                            });
                        },
                        []{ });
                });
                statement.assign(os.str());
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
            return _statement;
        }
    };

    template<typename T_modifiers>
    inline decltype(auto) build_order_by(const schema_t& schema, const T_modifiers& modifiers)
    {
        static order_by_builder<T_modifiers> builder;
        return builder.assign(schema, modifiers);
    }

}
end_namespace_cpphibernate_driver_mariadb
