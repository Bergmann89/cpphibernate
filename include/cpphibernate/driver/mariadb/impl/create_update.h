#pragma once

#include <cppmariadb.h>
#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* create_update_impl_t */

    template<typename T_dataset, typename = void>
    struct create_update_impl_t
    {
        using dataset_type          = T_dataset;
        using reference_stack_type  = reference_stack<dataset_type>;

        static inline value_t apply(dataset_type& dataset, const create_update_context& context, bool strict = true)
        {
            value_t ret;

            auto  dataset_id    = misc::get_type_id(hana::type_c<dataset_type>);
            auto& connection    = context.connection;
            auto& schema        = context.schema;
            auto& table         = schema.table(dataset_id);
            auto  ref_lock      = reference_stack_type::push(dataset);

            assert(table.primary_key_field);
            transaction_lock trans(connection);
            if (table.primary_key_field->is_default())
            {
                ret = table.create_update(context);
            }
            else if (strict)
            {
                throw misc::hibernate_exception("dataset have already a primary key assigned!");
            }
            else
            {
                auto update_context = context;
                update_context.is_update = true;
                ret = table.create_update(update_context);
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

        static inline value_t apply(dataset_type& dataset, const create_update_context& context, bool strict = true)
        {
            value_t ret;
            auto* value = nullable_helper_type::get(dataset);

            if (value)
            {
                using new_dataset_type            = mp::decay_t<decltype(*value)>;
                using new_create_update_impl_type = create_update_impl_t<new_dataset_type>;
                ret = new_create_update_impl_type::apply(*value, context, strict);
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

        static inline value_t apply(dataset_type& dataset, const create_update_context& context, bool strict = true)
        {
            value_t ret;
            auto& connection = context.connection;

            transaction_lock trans(connection);
            for (auto& x : dataset)
            {
                using new_dataset_type            = mp::decay_t<decltype(x)>;
                using new_create_update_impl_type = create_update_impl_t<new_dataset_type>;
                new_create_update_impl_type::apply(x, context, strict);
            }
            trans.commit();
            return ret;
        }
    };

}
end_namespace_cpphibernate_driver_mariadb