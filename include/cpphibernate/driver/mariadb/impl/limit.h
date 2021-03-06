#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/modifier.h>
#include <cpphibernate/driver/mariadb/impl/modifier_tags.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* limit_builder */

    template<typename T_modifiers>
    struct limit_builder
    {
        ::cppmariadb::statement statement;

        limit_builder(const T_modifiers& p_modifier)
        {
            ssize_t limit = -1;
            ssize_t offset = -1;

            hana::for_each(p_modifier, [&limit, &offset](auto& x_modifier){
                using modifier_type  = mp::decay_t<decltype(x_modifier)>;
                using is_limit_type  = modifier::is_limit_modifier<modifier_type>;
                using is_offset_type = modifier::is_offset<modifier_type>;
                hana::eval_if(
                    is_limit_type { },
                    [&limit, &x_modifier](auto _){
                        limit = static_cast<ssize_t>(hana::value(_(x_modifier).value));
                    },
                    [&offset, &x_modifier](){
                        hana::eval_if(
                            is_offset_type { },
                            [&offset, &x_modifier](auto _){
                                offset = static_cast<ssize_t>(hana::value(_(x_modifier).value));
                            },
                            []{
                                /* no-op */
                            });
                    });
            });

            if (offset >= 0 && limit < 0)
                limit = 1000000;

            if (limit >= 0)
            {
                std::ostringstream ss;
                ss << "LIMIT " << limit;
                if (offset >= 0)
                    ss << " OFFSET " << offset;
                statement.assign(ss.str());
            }
        }
    };

    template<typename T_modifiers>
    inline ::cppmariadb::statement& build_limit(const T_modifiers& modifiers)
    {
        static limit_builder<T_modifiers> builder(modifiers);
        return builder.statement;
    }

}
end_namespace_cpphibernate_driver_mariadb
