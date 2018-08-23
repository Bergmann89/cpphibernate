#pragma once

#include <memory>
#include <vector>

#include <cppmariadb.h>
#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/schema/table.h>
#include <cpphibernate/schema/schema.h>
#include <cpphibernate/driver/mariadb/schema/fields.h>
#include <cpphibernate/driver/mariadb/schema/table.fwd.h>
#include <cpphibernate/driver/mariadb/helper/context.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* table_t */

    struct table_t
    {
        size_t                      dataset_id          { 0 };
        size_t                      base_dataset_id     { 0 };
        size_t                      table_id            { 0 };

        std::vector<size_t>         derived_dataset_ids;
        std::string                 table_name;
        std::string                 schema_name;

        fields_t                    fields;

        const table_t*              base_table          { nullptr };
        std::vector<const table_t*> derived_tables;

        const field_t*              primary_key_field   { nullptr };
        std::vector<const field_t*> foreign_key_fields;
        std::vector<const field_t*> foreign_table_fields;
        std::vector<const field_t*> foreign_table_one_fields;
        std::vector<const field_t*> foreign_table_many_fields;
        std::vector<const field_t*> data_fields;

        inline table_t() = default;
        inline table_t(const table_t&) = delete;
        inline table_t(table_t&& other)
            : dataset_id                (std::move(other).dataset_id)
            , base_dataset_id           (std::move(other).base_dataset_id)
            , table_id                  (std::move(other).table_id)
            , derived_dataset_ids       (std::move(other).derived_dataset_ids)
            , table_name                (std::move(other).table_name)
            , schema_name               (std::move(other).schema_name)
            , fields                    (std::move(other).fields)
            , base_table                (nullptr)
            , derived_tables            ()
            , primary_key_field         (nullptr)
            , foreign_key_fields        ()
            , foreign_table_fields      ()
            , foreign_table_one_fields  ()
            , foreign_table_many_fields ()
            , data_fields               ()
            { }
        virtual ~table_t() { };

        void print(std::ostream& os) const;

        /* CRUD */
        inline void init(const init_context& context) const
            { return init_intern(context); }

    private:
        using statement_ptr = std::unique_ptr<::cppmariadb::statement>;

        mutable statement_ptr _statement_create_table;

        ::cppmariadb::statement& get_statement_create_table() const;

    protected:
        void init_intern(const init_context& context) const;
    };

    /* table_simple_t */

    template<typename T_schema, typename T_table, typename T_base_dataset>
    struct table_simple_t
        : public table_t
    {
        using schema_type       = T_schema;
        using table_type        = T_table;
        using base_dataset_type = T_base_dataset;

        const schema_type&  schema;
        const table_type&   table;

        inline table_simple_t(const schema_type& p_schema, const table_type& p_table)
            : schema(p_schema)
            , table (p_table)
            { }

        inline table_simple_t(const table_simple_t&) = delete;

        inline table_simple_t(table_simple_t&& other)
            : table_t(std::move(other))
            , schema (std::move(other).schema)
            , table  (std::move(other).table)
            { }
    };

    /* table_polymorphic_t */

    template<typename T_schema, typename T_table, typename T_base_dataset>
    struct table_polymorphic_t
        : public table_simple_t<T_schema, T_table, T_base_dataset>
    {
        using base_type         = table_simple_t<T_schema, T_table, T_base_dataset>;
        using schema_type       = T_schema;
        using table_type        = T_table;
        using base_dataset_type = T_base_dataset;

        using base_type::base_type;

        const schema_type&  schema;
        const table_type&   table;
    };

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
                using table_type                    = table_type_t<dataset_type, base_type>;
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