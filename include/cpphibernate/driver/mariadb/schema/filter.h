#pragma once

#include <set>
#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/schema/field.fwd.h>
#include <cpphibernate/driver/mariadb/schema/table.fwd.h>
#include <cpphibernate/driver/mariadb/schema/filter.fwd.h>
#include <cpphibernate/driver/mariadb/schema/schema.fwd.h>

beg_namespace_cpphibernate_driver_mariadb
{

    namespace __impl
    {

        /* filter_add_element */

        template<typename T, typename = void>
        struct filter_add_element_impl;

        constexpr decltype(auto) filter_add_element = misc::make_generic_predicate<__impl::filter_add_element_impl> { };

    }

    /* filter_t */

    struct filter_t
    {
    public:
        using field_set_type = std::set<const field_t*>;
        using table_set_type = std::set<const table_t*>;

        size_t          cache_id { 0 };
        bool            exclusive;
        field_set_type  fields;
        table_set_type  tables;

        inline bool is_excluded(const table_t& table) const;
        inline bool is_excluded(const field_t& field) const;

        template<typename... T_args>
        inline void set_inclusive(const schema_t& schema, T_args&&... args);

        template<typename... T_args>
        inline void set_exclusive(const schema_t& schema, T_args&&... args);

        inline void clear();

    private:
        void update_tables();
    };

}
end_namespace_cpphibernate_driver_mariadb