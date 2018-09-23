#pragma once

#include <memory>

#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/schema/table.h>
#include <cpphibernate/schema/tables.h>
#include <cpphibernate/modifier/where.h>
#include <cpphibernate/modifier/modifiers.h>

beg_namespace_cpphibernate
{

    namespace __impl
    {

        template<typename T_driver, typename T_schema>
        struct context_t
            : public T_driver
        {
        public:
            using base_type   = T_driver;
            using driver_type = T_driver;
            using schema_type = T_schema;

        private:
            const schema_type& _schema;

        public:
            template<typename... T_args>
            constexpr context_t(const schema_type& p_schema, T_args&&... args)
                : base_type (p_schema, std::forward<T_args>(args)...)
                , _schema   (p_schema)
                { }

            cpphibernate_copyable(context_t, delete);
            cpphibernate_moveable(context_t, default);

            /* init */

            inline void init(bool recreate)
                { this->init_impl(recreate); }

            /* create */

            template<typename T_dataset>
            constexpr void create(T_dataset& dataset)
                { this->create_impl(dataset); }

            /* read */

            template<typename T_dataset, typename... T_modifiers>
            constexpr auto read(T_dataset& dataset, T_modifiers&&... modifiers)
                -> mp::enable_if<
                    modifier::all_are_modifiers<mp::decay_t<T_modifiers>...>>
            {
                using namespace modifier;
                using real_dataset_type = misc::real_dataset_t<mp::decay_t<T_dataset>>;
                schema::tables::find(_schema.tables, hana::type_c<real_dataset_type>);
                this->read_impl(dataset, modifier::make_list(std::forward<T_modifiers>(modifiers)...));
            }

            template<typename T_dataset>
            constexpr auto read(T_dataset& dataset)
                -> mp::enable_if_c<
                       !misc::is_container<mp::decay_t<T_dataset>>::value
                    && !misc::is_nullable<mp::decay_t<T_dataset>>::value>
            {
                using namespace modifier;
                using real_dataset_type = misc::real_dataset_t<mp::decay_t<T_dataset>>;
                auto& table         = schema::tables::find(_schema.tables, hana::type_c<real_dataset_type>);
                auto& primary_key   = schema::table::get_primary_key_field(table);
                this->read_impl(dataset, modifier::make_list(where(equal(primary_key, primary_key.getter(dataset)))));
            }

            template<typename T_dataset>
            constexpr auto read(T_dataset& dataset)
                -> mp::enable_if_c<
                        misc::is_container<mp::decay_t<T_dataset>>::value
                    ||  misc::is_nullable<mp::decay_t<T_dataset>>::value>
            {
                using namespace modifier;
                using real_dataset_type = misc::real_dataset_t<mp::decay_t<T_dataset>>;
                schema::tables::find(_schema.tables, hana::type_c<real_dataset_type>);
                this->read_impl(dataset, modifier::make_list());
            }

            /* update */

            template<typename T_dataset>
            constexpr void update(T_dataset& dataset)
                { this->update_impl(dataset); }

            /* destroy */

            template<typename T_dataset>
            constexpr void destroy(T_dataset& dataset)
                { this->destroy_impl(dataset); }
        };

    }

    /* make */

    template<typename T_driver, typename T_schema, typename... T_args>
    constexpr decltype(auto) make_context(T_schema&& schema, T_args&&... args)
    {
        using context_type = __impl::context_t<T_driver, T_schema>;
        return context_type(std::forward<T_schema>(schema), std::forward<T_args>(args)...);
    }

    template<typename T_driver, typename T_schema, typename... T_args>
    constexpr decltype(auto) make_context_ptr(T_schema&& schema, T_args&&... args)
    {
        using context_type = __impl::context_t<T_driver, T_schema>;
        using pointer_type = std::unique_ptr<context_type>;
        return pointer_type(new context_type(std::forward<T_schema>(schema), std::forward<T_args>(args)...));
    }


}
end_namespace_cpphibernate