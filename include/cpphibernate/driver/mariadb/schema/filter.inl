#pragma once

#include <cpphibernate/driver/mariadb/schema/filter.h>
#include <cpphibernate/driver/mariadb/schema/schema.inl>

beg_namespace_cpphibernate_driver_mariadb
{

    namespace __impl
    {

        /* filter_add_element_impl */

        template<typename T, typename>
        struct filter_add_element_impl
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for mariadb::filter_add_element(...)!"); }
        };

        template<typename T_table>
        struct filter_add_element_impl<
            mp::list<filter_t&, const schema_t&, T_table>,
            mp::enable_if_c<
                schema::is_table<mp::decay_t<T_table>>::value>>
        {
            static constexpr decltype(auto) apply(filter_t& filter, const schema_t& schema, const T_table& table)
            {
                auto  dataset_id = misc::get_type_id(table.wrapped_dataset);
                auto& t          = schema.table(dataset_id);
                filter.tables.emplace(&t);
            }
        };

        template<typename T_field>
        struct filter_add_element_impl<
            mp::list<filter_t&, const schema_t&, T_field>,
            mp::enable_if_c<
                schema::is_field<mp::decay_t<T_field>>::value>>
        {
            static constexpr decltype(auto) apply(filter_t& filter, const schema_t& schema, const T_field& field)
            {
                auto  field_id = misc::get_type_id(hana::type_c<mp::decay_t<T_field>>);
                auto& f        = schema.field(field_id);
                filter.fields.emplace(&f);
                filter.tables.emplace(f.table);
            }
        };

    }

    /* filter_t */

    bool filter_t::is_excluded(const table_t& table) const
    {
        auto ret = static_cast<bool>(tables.count(&table));
        if (!exclusive)
            ret = !ret;
        return ret;
    }

    bool filter_t::is_excluded(const field_t& field) const
    {
        auto ret = static_cast<bool>(fields.count(&field));
        if (!exclusive)
            ret = !ret;
        return ret;
    }

    template<typename... T_args>
    void filter_t::set_inclusive(const schema_t& schema, T_args&&... args)
    {
        clear();
        exclusive = false;
        cache_id  = static_cast<size_t>(utl::get_unique_id<filter_t, mp::decay_t<T_args>...>());
        int dummy[] = { 0, (__impl::filter_add_element(*this, schema, std::forward<T_args>(args)), void(), 0)... };
        (void)dummy;
    }

    template<typename... T_args>
    void filter_t::set_exclusive(const schema_t& schema, T_args&&... args)
    {
        clear();
        exclusive = true;
        cache_id  = static_cast<size_t>(utl::get_unique_id<filter_t, mp::decay_t<T_args>...>());
        int dummy[] = { 0, (__impl::filter_add_element(*this, schema, std::forward<T_args>(args)), void(), 0)... };
        (void)dummy;

        // remove excluded tables if not all fields are excluded
        auto it = tables.begin();
        while (it != tables.end())
        {
            for (auto& field : (*it)->fields)
            {
                if (fields.count(field.get()))
                {
                    it = tables.erase(it);
                    continue;
                }
            }
            ++it;
        }
    }

    void filter_t::clear()
    {
        cache_id  = 0;
        exclusive = true;
        fields.clear();
        tables.clear();
    }

}
end_namespace_cpphibernate_driver_mariadb