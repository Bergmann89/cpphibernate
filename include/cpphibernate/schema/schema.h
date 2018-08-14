#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/misc/general.h>
#include <cpphibernate/schema/tables.h>

beg_namespace_cpphibernate_schema
{

    namespace __impl
    {

        /* schema_t */

        template<typename T_name, typename T_tables>
        struct schema_t
        {
            using name_type     = T_name;
            using tables_type   = T_tables;

            name_type   name;
            tables_type tables;

            constexpr schema_t(
                T_name&&    p_name,
                T_tables&&  p_tables)
                : name  (std::forward<T_name>  (p_name))
                , tables(std::forward<T_tables>(p_tables))
                { }

            cpphibernate_copyable(schema_t, delete);
            cpphibernate_moveable(schema_t, default);

            inline void print(std::ostream& os) const
            {
                using namespace ::utl;
                size_t index = 0;
                os  << indent << '{'
                    << incindent
                        << indent << "\"name\": \"" << name << "\","
                        << indent << "\"tables\": "
                        << indent << '['
                        << incindent;
                        hana::for_each(tables, [&](auto& table){
                            if (index++ > 0) os << ",";
                            table.print(os);
                        });
                os      << decindent
                        << indent << ']'
                    << decindent
                    << indent << '}';
            }
        };

        /* schema_builder */

        template <typename X, typename = void>
        struct schema_builder
        {
            template <typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for hibernate::schema::make_schema(...)!"); }
        };

        template<typename T_name, typename T_tables>
        struct schema_builder<mp::list<T_name, T_tables>, mp::enable_if_c<
            is_tables<mp::clean_type<T_tables>>::value>>
        {
            static constexpr decltype(auto) apply(T_name&& name, T_tables&& tables)
                { return schema_t<T_name, T_tables>(std::forward<T_name>(name), std::forward<T_tables>(tables)); }
        };

    }

    /* meta */

    template<typename T>
    struct is_schema : mp::is_specialization_of<T, __impl::schema_t> { };

    /* make */

    constexpr decltype(auto) make_schema = misc::make_generic_predicate<__impl::schema_builder> { };

}
end_namespace_cpphibernate_schema