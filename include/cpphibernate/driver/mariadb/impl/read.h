#pragma once

#include <cppmariadb.h>
#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/helper.h>
#include <cpphibernate/driver/mariadb/schema/schema.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* read_impl_t */

    template<typename T_dataset, typename = void>
    struct read_impl_t
    {
        using dataset_type = T_dataset;

        struct context_impl
            : public read_context
        {
            mutable size_t count;

            template<typename T_read_context>
            context_impl(T_read_context&& p_read_context)
                : read_context  (std::forward<T_read_context>(p_read_context))
                , count         (0)
                { }

        private:
            virtual void* emplace_intern(void* data) const override
            {
                if (data)
                    throw misc::hibernate_exception("This context has a pre assigned dataset and can therefor not work in dynamic mode!");
                ++count;
                if (count > 1)
                    throw misc::hibernate_exception("Expected exactly one dataset, but received more!");
                return nullptr;
            }

            virtual void finish_intern() const override
            {
                if (count < 1)
                    throw misc::hibernate_exception("Expected exactly one dataset, but received none!");
            }
        };

        static inline void apply(const read_context& context)
        {
            auto  dataset_id = misc::get_type_id(hana::type_c<dataset_type>);
            auto& connection = context.connection;
            auto& schema     = context.schema;
            auto& table      = schema.table(dataset_id);

            transaction_lock trans(connection);
            table.read(context_impl(context));
            trans.commit();
        }
    };

    /* read_impl_t - nullable */

    template<typename T_dataset>
    struct read_impl_t<
        T_dataset,
        mp::enable_if<misc::is_nullable<T_dataset>>>
    {
        using dataset_type          = T_dataset;
        using nullable_helper_type  = misc::nullable_helper<dataset_type>;

        static inline void apply(const read_context& context)
        {

        }
    };

    /* read_impl_t - container */

    template<typename T_dataset>
    struct read_impl_t<
        T_dataset,
        mp::enable_if<misc::is_container<T_dataset>>>
    {
        using dataset_type = T_dataset;

        static inline void apply(const read_context& context)
        {

        }
    };

}
end_namespace_cpphibernate_driver_mariadb