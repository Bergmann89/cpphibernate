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
        auto& dataset = context.get<dataset_type>();
        for_each_derived(dataset, hana::false_c, [&](auto& derived_dataset){
            if (!done)
            {
                using derived_dataset_type  = mp::decay_t<decltype(derived_dataset)>;
                auto  derived_dataset_id    = misc::get_type_id(hana::type_c<derived_dataset_type>);
                auto  derived_table         = this->get_derived(derived_dataset_id);
                if (!derived_table)
                {
                    throw misc::hibernate_exception(static_cast<std::ostringstream&>(std::ostringstream { }
                        << "unable to find derived table info for dataset '"
                        << utl::type_helper<derived_dataset_type>::name() << "'!").str());
                }
                derived_table->create_update(change_context(context, derived_dataset));
                done = true;
            }
        });

        return done
            ? *this->primary_key_field->get(context)
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
                auto& dataset = context.get<dataset_type>();
                auto& base    = static_cast<tmp_type&>(dataset);
                return this->base_table->create_update_exec(change_context(context, base));
            });
    }

    namespace __impl
    {

        /* make_dataset_id_vector */

        struct make_dataset_id_vector_impl
        {
            template<typename T_wrapped_datasets, size_t... I>
            static constexpr decltype(auto) helper(const T_wrapped_datasets& wrapped_datasets, std::index_sequence<I...>)
            {
                return std::vector<size_t>({
                    misc::get_type_id(wrapped_datasets[hana::size_c<I>])...
                });
            }

            template<typename T_wrapped_datasets>
            constexpr decltype(auto) operator()(T_wrapped_datasets&& wrapped_datasets) const
            {
                using size = mp::decay_t<decltype(hana::size(wrapped_datasets))>;
                return helper(std::forward<T_wrapped_datasets>(wrapped_datasets), std::make_index_sequence<size::value> { });
            }
        };

        static constexpr decltype(auto) make_dataset_id_vector = make_dataset_id_vector_impl { };

        /* make_table_impl  */

        template<typename T, typename>
        struct make_table_impl
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == 0, "Invalid parameters for mariadb::make_table(...)!"); }
        };

        template<typename T_schema, typename T_table>
        struct make_table_impl<
            mp::list<T_schema, T_table>,
            mp::enable_if_c<
                    schema::is_schema<mp::decay_t<T_schema>>::value
                &&  schema::is_table <mp::decay_t<T_table>>::value>>
        {

            /* table_type */

            template<typename T_dataset, typename T_base_dataset, typename = void>
            struct table_type
                { using type = table_simple_t<mp::decay_t<T_schema>, mp::decay_t<T_table>, T_base_dataset>; };

            template<typename T_dataset, typename T_base_dataset>
            struct table_type<T_dataset, T_base_dataset, mp::enable_if_c<
                std::is_polymorphic<T_dataset>::value>>
                { using type = table_polymorphic_t<mp::decay_t<T_schema>, mp::decay_t<T_table>, T_base_dataset>; };

            template<typename T_dataset, typename T_base_dataset>
            using table_type_t = typename table_type<T_dataset, T_base_dataset>::type;

            /* apply */

            static decltype(auto) apply(const T_schema& schema, const T_table& table)
            {
                using wrapped_base_type             = mp::decay_t<decltype(
                                                        schema::get_base_type(
                                                            std::declval<T_schema>(),
                                                            std::declval<T_table>().wrapped_dataset))>;
                using base_type                     = misc::unwrap_t<wrapped_base_type>;
                using derived_wrapped_types_type    = mp::decay_t<decltype(
                                                        schema::get_derived_types(
                                                            std::declval<T_schema>(),
                                                            std::declval<T_table>().wrapped_dataset))>;
                using wrapped_dataset_type          = typename mp::decay_t<T_table>::wrapped_dataset_type;
                using dataset_type                  = misc::unwrap_t<wrapped_dataset_type>;
                using real_dataset_type             = misc::real_dataset_t<dataset_type>;
                using table_type                    = table_type_t<dataset_type, base_type>;

                static_assert(mp::is_same<dataset_type, real_dataset_type>::value, "table cn only be created for simple dataset types (not for containers)!");

                table_type ret(schema, table);
                ret.dataset_id          = misc::get_type_id(table.wrapped_dataset);
                ret.base_dataset_id     = misc::get_type_id(wrapped_base_type { });
                ret.derived_dataset_ids = make_dataset_id_vector(derived_wrapped_types_type { });
                ret.table_id            = hana::value(table.table_id);
                ret.schema_name         = schema.name;
                ret.table_name          = table.name;
                ret.fields              = make_fields(schema, table);
                return ret;
            }
        };

    }

}
end_namespace_cpphibernate_driver_mariadb