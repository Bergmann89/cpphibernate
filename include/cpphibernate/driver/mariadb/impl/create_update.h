#pragma once

#include <cppmariadb.h>
#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/helper.h>
#include <cpphibernate/driver/mariadb/schema/schema.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* create_update_impl_t */

    template<typename T_dataset, typename = void>
    struct create_update_impl_t
    {
        using dataset_type = T_dataset;

        static inline value_t apply(const create_update_context& context, bool strict = true)
        {
            value_t ret;

            auto  dataset_id = misc::get_type_id(hana::type_c<dataset_type>);
            auto& connection = context.connection;
            auto& schema     = context.schema;
            auto& table      = schema.table(dataset_id);

            assert(table.primary_key_field);
            transaction_lock trans(connection);
            if (table.primary_key_field->is_default(context) == context.is_update)
            {
                if (!strict)
                {
                    auto update_context = context;
                    update_context.is_update = !update_context.is_update;
                    ret = table.create_update(update_context);
                }
                else if (context.is_update)
                {
                    throw misc::hibernate_exception("can not update dataset with no primary key assigned!");
                }
                else
                {
                    throw misc::hibernate_exception("can not create dataset with primary key assigned!");
                }
            }
            else
            {
                ret = table.create_update(context);
            }
            trans.commit();
            return ret;
        }
    };

    /* create_update_impl_t - nullable */

    template<typename T_dataset>
    struct create_update_impl_t<
        T_dataset,
        mp::enable_if<misc::is_nullable<T_dataset>>>
    {
        using dataset_type          = T_dataset;
        using nullable_helper_type  = misc::nullable_helper<dataset_type>;

        static inline value_t apply(const create_update_context& context, bool strict = true)
        {
            value_t ret;
            auto& dataset   = context.get<dataset_type>();
            auto* value     = nullable_helper_type::get(dataset);

            if (value)
            {
                using new_dataset_type            = mp::decay_t<decltype(*value)>;
                using new_create_update_impl_type = create_update_impl_t<new_dataset_type>;
                ret = new_create_update_impl_type::apply(change_context(context, *value), strict);
            }
            else if (strict)
            {
                throw misc::hibernate_exception("can not create nullable type with no value!");
            }
            return ret;
        }
    };

    /* create_update_impl_t - container */

    template<typename T_dataset>
    struct create_update_impl_t<
        T_dataset,
        mp::enable_if<misc::is_container<T_dataset>>>
    {
        using dataset_type = T_dataset;

        static inline value_t apply(const create_update_context& context, bool strict = true)
        {
            value_t ret;
            auto& connection = context.connection;
            auto& dataset    = context.get<dataset_type>();

            transaction_lock trans(connection);
            ssize_t index = 0;
            for (auto& x : dataset)
            {
                using new_dataset_type            = mp::decay_t<decltype(x)>;
                using new_create_update_impl_type = create_update_impl_t<new_dataset_type>;
                auto new_context = change_context(context, x);
                new_context.index = index++;
                new_create_update_impl_type::apply(new_context, strict);
            }
            trans.commit();
            return ret;
        }
    };

}
end_namespace_cpphibernate_driver_mariadb