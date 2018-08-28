#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/misc/general.h>
#include <cpphibernate/schema/tables.h>

beg_namespace_cpphibernate_schema
{

    namespace __impl
    {

        /* schema_t */

        template<typename T_name, typename T_tables>
        struct schema_t
        {
            using name_type     = T_name;
            using tables_type   = T_tables;

            name_type   name;
            tables_type tables;

            constexpr schema_t(
                T_name&&    p_name,
                T_tables&&  p_tables)
                : name  (std::forward<T_name>  (p_name))
                , tables(std::forward<T_tables>(p_tables))
                { }

            cpphibernate_copyable(schema_t, delete);
            cpphibernate_moveable(schema_t, default);

            inline void print(std::ostream& os) const
            {
                using namespace ::utl;
                size_t index = 0;
                os  << indent << '{'
                    << incindent
                        << indent << "\"name\": \"" << name << "\","
                        << indent << "\"tables\": "
                        << indent << '['
                        << incindent;
                        hana::for_each(tables, [&](auto& table){
                            if (index++ > 0) os << ",";
                            table.print(os);
                        });
                os      << decindent
                        << indent << ']'
                    << decindent
                    << indent << '}';
            }
        };

    }

    /* meta */

    template<typename T>
    struct is_schema : mp::is_specialization_of<T, __impl::schema_t> { };

    /* schema::make */

    namespace __impl
    {
        template <typename X, typename = void>
        struct schema_make_impl
        {
            template <typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for hibernate::schema::make(...)!"); }
        };

        template<typename T_name, typename T_tables>
        struct schema_make_impl<mp::list<T_name, T_tables>, mp::enable_if_c<
            is_tables<mp::decay_t<T_tables>>::value>>
        {
            static constexpr decltype(auto) apply(T_name&& name, T_tables&& tables)
                { return schema_t<T_name, T_tables>(std::forward<T_name>(name), std::forward<T_tables>(tables)); }
        };
    }

    constexpr decltype(auto) make = misc::make_generic_predicate<__impl::schema_make_impl> { };

    /* schema::get_base_type */

    namespace __impl
    {
        struct schema_get_base_type_impl
        {
            static constexpr decltype(auto) is_base_of_pred = hana::integral(hana::metafunction<mp::is_base_of>);

            struct do_fold_impl
            {
                template<typename T_type_1, typename T_type_2>
                constexpr decltype(auto) operator()(T_type_1&& type_1, T_type_2&& type_2) const
                {
                    auto check = hana::or_(is_base_of_pred(type_1, type_2), is_base_of_pred(type_2, type_1));
                    static_assert(decltype(check)::value, "A dataset must not have more than one base class!");
                    return hana::if_(is_base_of_pred(type_1, type_2), type_2, type_1);
                }
            };

            static constexpr decltype(auto) do_fold = do_fold_impl { };

            template<typename T_wrapped_datasets, typename T_wrapped_dataset>
            static constexpr decltype(auto) get_base_types(T_wrapped_datasets&& wrapped_datasets, T_wrapped_dataset&& wrapped_dataset)
            {
                return hana::filter(
                    std::forward<T_wrapped_datasets>(wrapped_datasets),
                    [&](auto type){
                        return hana::and_(
                            is_base_of_pred(type, wrapped_dataset),
                            hana::not_equal(type, wrapped_dataset));
                    });
            }

            template<typename T_schema, typename T_wrapped_dataset>
            constexpr decltype(auto) operator()(T_schema&& schema, T_wrapped_dataset&& wrapped_dataset) const
            {
                using wrapped_datasets_type = mp::decay_t<decltype(
                                                hana::transform(
                                                    std::declval<T_schema>().tables,
                                                    schema::table::get_wrapped_dataset))>;
                auto base_types = get_base_types(wrapped_datasets_type { }, std::forward<T_wrapped_dataset>(wrapped_dataset));
                return hana::eval_if(
                    hana::size(base_types) <= hana::size_c<0>,
                        [&](auto _){ return hana::type_c<void>; },
                        [&](auto _){ return hana::fold(_(base_types), do_fold); });
            }
        };
    }

    constexpr decltype(auto) get_base_type = __impl::schema_get_base_type_impl { };

    /* schema::get_root_base_type */

    namespace __impl
    {
        struct schema_get_root_base_type_impl
        {
            template<typename T_wrapped_dataset>
            struct is_base_pred
            {
                using dataset_type = misc::decay_unwrap_t<T_wrapped_dataset>;

                template<typename T_type>
                constexpr decltype(auto) operator()(T_type&&) const
                {
                    using type = misc::unwrap_t<T_type>;
                    return hana::bool_c<
                            std::is_base_of<type, dataset_type>::value
                        && !std::is_same   <type, dataset_type>::value>;
                }
            };

            template<typename T_schema, typename T_wrapped_dataset>
            constexpr decltype(auto) operator()(T_schema&& schema, T_wrapped_dataset&& wrapped_dataset) const
            {
                auto all_types = hana::transform(
                                    schema.tables,
                                    table::get_wrapped_dataset);
                auto base_type = hana::find_if(
                                    all_types,
                                    is_base_pred<T_wrapped_dataset> { });
                return hana::eval_if(
                    base_type != hana::nothing,
                    [&](auto _){
                        return schema_get_root_base_type_impl { }(schema, _(base_type).value());
                    },
                    [&]{
                        return wrapped_dataset;
                    });
            }
        };
    }

    constexpr decltype(auto) get_root_base_type = __impl::schema_get_root_base_type_impl { };

    /* schema::get_all_derived_types */

    namespace __impl
    {
        struct schema_get_all_derived_types_impl
        {
            template<typename T_wrapped_dataset>
            struct is_derived_pred
            {
                using dataset_type = misc::decay_unwrap_t<T_wrapped_dataset>;

                template<typename T_type>
                constexpr decltype(auto) operator()(T_type&&) const
                {
                    return hana::bool_c<
                        std::is_base_of<dataset_type, misc::decay_unwrap_t<T_type>>::value>;
                }
            };

            template<typename T_schema, typename T_wrapped_dataset>
            constexpr decltype(auto) operator()(T_schema&& schema, T_wrapped_dataset&&) const
            {
                auto all_types      = hana::transform(
                                        schema.tables,
                                        table::get_wrapped_dataset);
                auto derived_types  = hana::filter(
                                        all_types,
                                        is_derived_pred<T_wrapped_dataset> { });
                return derived_types;
            }
        };
    }

    constexpr decltype(auto) get_all_derived_types = __impl::schema_get_all_derived_types_impl { };

    /* schema::get_derived_types */

    namespace __impl
    {
        struct schema_get_derived_types_impl
        {
            struct has_base_impl
            {
                template<typename T_schema, typename T_base_type, typename T_type>
                constexpr decltype(auto) operator()(T_schema&& schema, T_base_type& base_type, T_type&& type) const
                    { return get_base_type(schema, type) == base_type; }
            };

            static constexpr decltype(auto) has_base = has_base_impl { };

            template<typename T_schema, typename T_wrapped_dataset>
            constexpr decltype(auto) operator()(T_schema&& schema, T_wrapped_dataset&& wrapped_dataset) const
            {
                using wrapped_datasets_type         = mp::decay_t<decltype(
                                                        hana::transform(
                                                            std::declval<T_schema>().tables,
                                                            schema::table::get_wrapped_dataset))>;
                using derived_wrapped_datasets_type = mp::decay_t<decltype(
                                                        hana::filter(
                                                            std::declval<wrapped_datasets_type>(),
                                                            hana::partial(
                                                                has_base,
                                                                std::declval<T_schema>(),
                                                                wrapped_dataset)))>;
                return derived_wrapped_datasets_type { };
            }
        };
    }

    constexpr decltype(auto) get_derived_types = __impl::schema_get_derived_types_impl { };

}
end_namespace_cpphibernate_schema