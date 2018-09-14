#pragma once

#include <cppmariadb.h>
#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/helper.h>
#include <cpphibernate/driver/mariadb/schema/schema.h>

beg_namespace_cpphibernate_driver_mariadb
{

    namespace __impl
    {

        template<typename T, typename = void>
        struct make_read_context_impl
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for mariadb::make_read_context(...)!"); }
        };

        template<typename T_dataset, typename... T_args>
        struct make_read_context_impl<
            mp::list<T_dataset, T_args...>,
            mp::enable_if_c<
                   !misc::is_container<mp::decay_t<T_dataset>>::value
                && !misc::is_nullable<mp::decay_t<T_dataset>>::value>>
        {
            using dataset_type = mp::decay_t<T_dataset>;

            struct context_impl
                : public read_context
            {
            private:
                mutable size_t  _count;
                dataset_type&   _dataset;

            public:
                template<typename... X_args>
                context_impl(dataset_type& dataset, X_args&&... args)
                    : read_context  (std::forward<X_args>(args)...)
                    , _count        (0)
                    , _dataset      (dataset)
                {
                    is_dynamic      = false;
                    base_dataset_id = misc::get_type_id(hana::type_c<dataset_type>);
                }

            private:
                virtual void* emplace_intern(void* data, size_t dataset_id) const override
                {
                    if (data || dataset_id != 0)
                        throw misc::hibernate_exception("Static datasets can not be assigned!");
                    ++_count;
                    if (_count > 1)
                        throw misc::hibernate_exception("Expected exactly one dataset, but received more!");
                    return set(_dataset);
                }

                virtual void finish_intern() const override
                {
                    if (_count < 1)
                        throw misc::hibernate_exception("Expected exactly one dataset, but received none!");
                }
            };

            static constexpr decltype(auto) apply(dataset_type& dataset, T_args&&... args)
                { return context_impl(dataset, std::forward<T_args>(args)...); }
        };

        template<typename T_dataset, typename... T_args>
        struct make_read_context_impl<
            mp::list<T_dataset, T_args...>,
            mp::enable_if_c<
                   !misc::is_container<mp::decay_t<T_dataset>>::value
                &&  misc::is_nullable<mp::decay_t<T_dataset>>::value>>
        {
            using dataset_type          = mp::decay_t<T_dataset>;
            using nullable_helper_type  = misc::nullable_helper<dataset_type>;
            using value_type            = typename nullable_helper_type::value_type;

            struct context_impl
                : public read_context
            {
            private:
                dataset_type&   _dataset;
                mutable size_t  _count;

            public:
                template<typename... X_args>
                context_impl(dataset_type& dataset, X_args&&... args)
                    : read_context  (std::forward<X_args>(args)...)
                    , _dataset      (dataset)
                    , _count        (0)
                {
                    is_dynamic      = misc::is_pointer<dataset_type>::value;
                    base_dataset_id = misc::get_type_id(hana::type_c<value_type>);
                    nullable_helper_type::clear(_dataset);
                }

            private:
                virtual void* emplace_intern(void* data, size_t dataset_id) const override
                {
                    if (data && !misc::is_pointer<dataset_type>::value)
                        throw misc::hibernate_exception("None pointer type can not be assigned!");
                    ++_count;
                    if (_count > 1)
                        throw misc::hibernate_exception("Expected exactly one dataset, but received more!");

                    if (data)
                    {
                        auto* cast  = static_cast<value_type*>(data);
                        auto& value = nullable_helper_type::set(_dataset, cast);
                        if (cast != &value)
                            throw misc::hibernate_exception("Nullable pointer value has changed!");
                        return set(value, dataset_id);
                    }
                    else
                    {
                        auto& value = nullable_helper_type::set(_dataset, value_type { });
                        return set(value);
                    }
                }

                virtual void finish_intern() const override
                    { }
            };

            static constexpr decltype(auto) apply(dataset_type& dataset, T_args&&... args)
                { return context_impl(dataset, std::forward<T_args>(args)...); }
        };

        template<typename T_dataset, typename... T_args>
        struct make_read_context_impl<
            mp::list<T_dataset, T_args...>,
            mp::enable_if_c<
                    misc::is_container<mp::decay_t<T_dataset>>::value
                && !misc::is_nullable<mp::decay_t<T_dataset>>::value>>
        {
            using dataset_type          = mp::decay_t<T_dataset>;
            using real_dataset_type     = misc::real_dataset_t<dataset_type>;
            using container_helper_type = misc::container_helper<dataset_type>;
            using value_type            = typename container_helper_type::value_type;

            struct context_impl
                : public read_context
            {
            private:
                dataset_type&   _dataset;
                mutable size_t  _count;

            public:
                template<typename... X_args>
                context_impl(dataset_type& dataset, X_args&&... args)
                    : read_context  (std::forward<X_args>(args)...)
                    , _dataset      (dataset)
                    , _count        (0)
                {
                    is_dynamic      = misc::is_pointer<value_type>::value;
                    base_dataset_id = misc::get_type_id(hana::type_c<real_dataset_type>);
                    container_helper_type::clear(_dataset);
                }

            private:
                virtual void* emplace_intern(void* data, size_t dataset_id) const override
                {
                    return hana::eval_if(
                        misc::is_nullable<value_type> { },
                        [this, &data, &dataset_id](auto _){
                            using nullable_type         = typename mp::decay_t<decltype(_(hana::type_c<value_type>))>::type;
                            using nullable_helper_type  = misc::nullable_helper<nullable_type>;
                            using inner_value_type      = typename nullable_helper_type::value_type;

                            if (!data)
                                throw misc::hibernate_exception("Expected dynamic data for pointer type!");
                            if (!misc::is_pointer<nullable_type>::value)
                                throw misc::hibernate_exception("None pointer type can not be assigned!");

                            auto& nullable = container_helper_type::emplace(this->_dataset);
                            auto* cast     = static_cast<inner_value_type*>(data);
                            auto& value    = nullable_helper_type::set(nullable, cast);
                            if (cast != &value)
                                throw misc::hibernate_exception("Nullable pointer value has changed!");
                            return set(value, dataset_id);
                        },
                        [this, &data, &dataset_id](){
                            if (data || dataset_id != 0)
                                throw misc::hibernate_exception("Static datasets can not be assigned!");
                            auto& value = container_helper_type::emplace(this->_dataset);
                            return this->set(value);
                        });
                }

                virtual void finish_intern() const override
                    { }
            };

            static constexpr decltype(auto) apply(dataset_type& dataset, T_args&&... args)
                { return context_impl(dataset, std::forward<T_args>(args)...); }
        };

    }

    constexpr decltype(auto) make_read_context = misc::make_generic_predicate<__impl::make_read_context_impl> { };

    namespace __impl
    {

        template<typename T, typename = void>
        struct make_fake_context_impl
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid parameters for mariadb::make_fake_context(...)!"); }
        };

        template<typename T_wrapped_dataset, typename... T_args>
        struct make_fake_context_impl<
            mp::list<T_wrapped_dataset, T_args...>,
            mp::enable_if_c<
                hana::is_a_t<hana::type_tag, T_wrapped_dataset>::value>>
        {
            using wrapped_dataset_type = mp::decay_t<T_wrapped_dataset>;
            using dataset_type         = misc::unwrap_t<wrapped_dataset_type>;
            using real_dataset_type    = misc::real_dataset_t<dataset_type>;

            struct context_impl
                : public read_context
            {
            public:
                template<typename... X_args>
                context_impl(X_args&&... args)
                    : read_context  (std::forward<X_args>(args)...)
                {
                    is_dynamic      = misc::is_pointer<dataset_type>::value;
                    base_dataset_id = misc::get_type_id(hana::type_c<real_dataset_type>);
                }

            private:
                virtual void* emplace_intern(void* data, size_t dataset_id) const override
                    { return nullptr; }

                virtual void finish_intern() const override
                    { }
            };

            static constexpr decltype(auto) apply(T_wrapped_dataset&&, T_args&&... args)
                { return context_impl(std::forward<T_args>(args)...); }
        };

    }

    constexpr decltype(auto) make_fake_context = misc::make_generic_predicate<__impl::make_fake_context_impl> { };

}
end_namespace_cpphibernate_driver_mariadb