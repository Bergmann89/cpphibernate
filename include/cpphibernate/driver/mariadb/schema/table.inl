#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/schema/table.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* table_polymorphic_t */

    template<typename T_schema, typename T_table, typename T_base_dataset>
    template<typename T_dataset, typename T_pred, typename T_include_self>
    constexpr void table_polymorphic_t<T_schema, T_table, T_base_dataset>
        ::for_each_derived(T_dataset& dataset, const T_include_self& include_self, const T_pred& pred) const
    {
        auto derived_types = hana::filter(
            schema::get_all_derived_types(this->schema, hana::type_c<dataset_type>),
            [&](auto type){
                return hana::and_(
                    hana::not_(hana::trait<std::is_abstract>(type)),
                    hana::or_(
                        type != hana::type_c<dataset_type>,
                        include_self));
            });

        hana::for_each(derived_types, [&](auto& type){
            using derived_type = misc::decay_unwrap_t<decltype(type)>;
            auto* derived      = dynamic_cast<derived_type*>(&dataset);
            if (derived)
                pred(*derived);
        });
    }

    template<typename T_schema, typename T_table, typename T_base_dataset>
    std::string table_polymorphic_t<T_schema, T_table, T_base_dataset>
        ::create_update_intern(const create_update_context& context) const
    {
        bool  done    = false;
        auto& dataset = reference_stack<dataset_type>::top();
        for_each_derived(dataset, hana::false_c, [&](auto& derived_dataset){
            if (!done)
            {
                using derived_dataset_type  = mp::decay_t<decltype(derived_dataset)>;
                using reference_stack_type  = reference_stack<derived_dataset_type>;
                auto  derived_dataset_id    = misc::get_type_id(hana::type_c<derived_dataset_type>);
                auto  derived_table         = this->get_derived(derived_dataset_id);
                if (!derived_table)
                {
                    throw misc::hibernate_exception(static_cast<std::ostringstream&>(std::ostringstream { }
                        << "unable to find derived table info for dataset '"
                        << utl::type_helper<derived_dataset_type>::name() << "'!").str());
                }
                auto ref_lock = reference_stack_type::push(derived_dataset);
                derived_table->create_update(context);
                done = true;
            }
        });

        return done
            ? *this->primary_key_field->get()
            : this->create_update_exec(context);
    }

    template<typename T_schema, typename T_table, typename T_base_dataset>
    std::string table_polymorphic_t<T_schema, T_table, T_base_dataset>
        ::create_update_base(const create_update_context& context) const
    {
        return hana::eval_if(
            mp::is_same<base_dataset_type, void> { },
            [this]()->std::string {
                throw misc::hibernate_exception(static_cast<std::ostringstream&>(std::ostringstream { }
                    << "'" << this->table_name << "' does not have a base table").str());
            },
            [this, &context](auto _)->std::string {
                using tmp_type = misc::decay_unwrap_t<decltype(_(hana::type_c<base_dataset_type>))>;
                assert(base_table);
                auto& dataset = reference_stack<dataset_type>::top();
                auto& base    = static_cast<tmp_type&>(dataset);
                auto  lock    = reference_stack<tmp_type>::push(base);
                return this->base_table->create_update_exec(context);
            });
    }

}
end_namespace_cpphibernate_driver_mariadb