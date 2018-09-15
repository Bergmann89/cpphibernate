#pragma once

#include <cppmariadb.h>
#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/helper.h>
#include <cpphibernate/driver/mariadb/schema/schema.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* destroy_impl_t */

    template<typename T_dataset, typename = void>
    struct destroy_impl_t
    {
        using dataset_type = T_dataset;

        static inline void apply(const destroy_context& context, bool strict = true)
        {
            auto  dataset_id = misc::get_type_id(hana::type_c<dataset_type>);
            auto& connection = context.connection;
            auto& schema     = context.schema;
            auto& table      = schema.table(dataset_id);

            assert(table.primary_key_field);
            transaction_lock trans(connection);
            if (!table.primary_key_field->is_default(context))
            {
                table.destroy(context);
            }
            else if (strict)
            {
                throw misc::hibernate_exception("can not destroy dataset with no primary key assigned!");
            }
            trans.commit();
        }
    };

    /* destroy_impl_t - nullable */

    template<typename T_dataset>
    struct destroy_impl_t<
        T_dataset,
        mp::enable_if<misc::is_nullable<T_dataset>>>
    {
        using dataset_type          = T_dataset;
        using nullable_helper_type  = misc::nullable_helper<dataset_type>;

        static inline void apply(const destroy_context& context, bool strict = true)
        {
            auto& dataset   = context.get<dataset_type>();
            auto* value     = nullable_helper_type::get(dataset);

            if (value)
            {
                using new_dataset_type      = mp::decay_t<decltype(*value)>;
                using new_destroy_impl_type = destroy_impl_t<new_dataset_type>;
                auto  new_context           = change_context(context, *value);
                new_destroy_impl_type::apply(new_context, strict);
            }
            else if (strict)
            {
                throw misc::hibernate_exception("can not destroy nullable type with no value!");
            }
        }
    };

    /* destroy_impl_t - container */

    template<typename T_dataset>
    struct destroy_impl_t<
        T_dataset,
        mp::enable_if<misc::is_container<T_dataset>>>
    {
        using dataset_type = T_dataset;

        static inline void apply(const destroy_context& context, bool strict = true)
        {
            auto& connection = context.connection;
            auto& dataset    = context.get<dataset_type>();

            transaction_lock trans(connection);
            for (auto& x : dataset)
            {
                using new_dataset_type      = mp::decay_t<decltype(x)>;
                using new_destroy_impl_type = destroy_impl_t<new_dataset_type>;
                auto  new_context           = change_context(context, x);
                new_destroy_impl_type::apply(new_context, strict);
            }
            trans.commit();
        }
    };

}
end_namespace_cpphibernate_driver_mariadb