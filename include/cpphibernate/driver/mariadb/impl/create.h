#pragma once

#include <cppmariadb.h>
#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* create_impl_t */

    template<typename T_context, typename = void>
    struct create_impl_t
    {
        using context_type          = T_context;
        using dataset_type          = typename context_type::dataset_type;
        using reference_stack_type  = reference_stack<dataset_type>;

        static inline value_t apply(const context_type& context, bool strict = true)
        {
            value_t ret;
            auto  dataset_id    = misc::get_type_id(hana::type_c<dataset_type>);
            auto& connection    = context.connection;
            auto& schema        = context.schema;
            auto& table         = schema.table(dataset_id);
            auto& dataset       = context.dataset;
            auto  ref_lock      = reference_stack_type::push(dataset);

            transaction_lock trans(connection);
            ret = table.create(context);
            trans.commit();
            return ret;
        }
    };

    /* create_impl_t - nullable */

    template<typename T_context>
    struct create_impl_t<
        T_context,
        mp::enable_if<misc::is_nullable<typename T_context::dataset_type>>>
    {
        using context_type          = T_context;
        using dataset_type          = typename context_type::dataset_type;
        using nullable_helper_type  = misc::nullable_helper<dataset_type>;

        static inline value_t apply(const context_type& context, bool strict = true)
        {
            value_t ret;
            auto& dataset = context.dataset;
            auto* value = nullable_helper_type::get(dataset);

            if (value)
            {
                using new_context_type      = mp::decay_t<decltype(context.change(*value))>;
                using new_create_impl_type  = create_impl_t<new_context_type>;
                ret = new_create_impl_type::apply(context.change(*value));
            }
            else if (strict)
            {
                throw misc::hibernate_exception("can not create nullable type with no value!");
            }
            return ret;
        }
    };

    /* create_impl_t - container */

    template<typename T_context>
    struct create_impl_t<
        T_context,
        mp::enable_if<misc::is_container<typename T_context::dataset_type>>>
    {
        using context_type = T_context;

        static inline value_t apply(const context_type& context, bool strict = true)
        {
            value_t ret;
            auto& connection = context.connection;
            auto& dataset    = context.dataset;

            transaction_lock trans(connection);
            for (auto& x : dataset)
            {
                using new_context_type     = mp::decay_t<decltype(context.change(x))>;
                using new_create_impl_type = create_impl_t<new_context_type>;
                new_create_impl_type::apply(context.change(x, context.owner_field));
            }
            trans.commit();
            return ret;
        }
    };

}
end_namespace_cpphibernate_driver_mariadb