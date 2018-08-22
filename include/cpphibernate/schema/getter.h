#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/misc/general.h>

beg_namespace_cpphibernate_schema
{

    namespace __impl
    {

        /* getter_t */

        template<typename T_dataset, typename T_value>
        struct getter_t
        {
            using dataset_type  = T_dataset;
            using value_type    = T_value;
        };

        /* getter_member_var_t */

        template<typename T_dataset, typename T_value, typename T_member>
        struct getter_member_var_t
            : public getter_t<T_dataset, T_value>
        {
            using base_type     = getter_t<T_dataset, T_value>;
            using dataset_type  = typename base_type::dataset_type;
            using value_type    = typename base_type::value_type;
            using member_type   = T_member;

            member_type member;

            template<typename X_member>
            constexpr getter_member_var_t(X_member&& p_member)
                : member(std::forward<X_member>(p_member))
                { }

            cpphibernate_copyable(getter_member_var_t, delete);
            cpphibernate_moveable(getter_member_var_t, default);

            template<typename X_dataset>
            constexpr decltype(auto) operator()(X_dataset&& data) const
                { return std::forward<X_dataset>(data).*member; }
        };

        /* getter_member_func_t */

        template<typename T_dataset, typename T_value, typename T_member>
        struct getter_member_func_t
            : public getter_t<T_dataset, T_value>
        {
            using base_type     = getter_t<T_dataset, T_value>;
            using dataset_type  = typename base_type::dataset_type;
            using value_type    = typename base_type::value_type;
            using member_type   = T_member;

            member_type member;

            template<typename X_member>
            constexpr getter_member_func_t(X_member&& p_member)
                : member(std::forward<X_member>(p_member))
                { };

            cpphibernate_copyable(getter_member_func_t, delete);
            cpphibernate_moveable(getter_member_func_t, default);

            template<typename X_dataset>
            constexpr decltype(auto) operator()(X_dataset&& data) const
                { return (std::forward<X_dataset>(data).*member)(); }
        };

        /* getter_lambda_t */

        template<typename T_dataset, typename T_lambda>
        struct getter_lambda_t
            : public getter_t<T_dataset, decltype(std::declval<T_lambda>()(std::declval<T_dataset>()))>
        {
            using base_type     = getter_t<T_dataset, decltype(std::declval<T_lambda>()(std::declval<T_dataset>()))>;
            using dataset_type  = typename base_type::dataset_type;
            using value_type    = typename base_type::value_type;
            using lambda_type   = T_lambda;

            lambda_type lambda;

            template<typename X_lambda>
            constexpr getter_lambda_t(X_lambda&& p_lambda)
                : lambda(std::forward<X_lambda>(p_lambda))
                { }

            cpphibernate_copyable(getter_lambda_t, delete);
            cpphibernate_moveable(getter_lambda_t, default);

            template<typename X_dataset>
            constexpr decltype(auto) operator()(X_dataset&& data) const
                { return lambda(std::forward<X_dataset>(data)); }
        };

        /* is_getter_impl */

        template<typename T, typename = void>
        struct is_getter_impl
            : public mp::c_false_t
            { };

        template<typename T>
        struct is_getter_impl<T, mp::enable_if_c<
                mp::is_base_of<getter_t<typename T::dataset_type, typename T::value_type>, T>::value>>
            : public mp::c_true_t
            { };

    }

    /* meta */

    template<typename T>
    struct is_getter :
        public __impl::is_getter_impl<T>
        { };

    /* make */

    template<typename T_dataset, typename T_value>
    constexpr decltype(auto) make_getter_member_var(T_value T_dataset::* member)
        { return __impl::getter_member_var_t<T_dataset, T_value, T_value T_dataset::*>(member); }

    template<typename T_dataset, typename T_value>
    constexpr decltype(auto) make_getter_member_func(T_value (T_dataset::*member)())
        { return __impl::getter_member_func_t<T_dataset, T_value, T_value (T_dataset::*)()>(member); }

    template<typename T_dataset, typename T_value>
    constexpr decltype(auto) make_getter_member_func(T_value (T_dataset::*member)() const)
        { return __impl::getter_member_func_t<const T_dataset, T_value, T_value (T_dataset::*)() const>(member); }

    template<typename T_dataset, typename T_lambda>
    constexpr decltype(auto) make_getter_lambda(T_lambda&& lambda, boost::hana::basic_type<T_dataset>)
        { return __impl::getter_lambda_t<T_dataset, T_lambda>(std::forward<T_lambda>(lambda)); }

    /* operations */

    namespace __impl
    {

        /* getter_make_impl */

        template<typename X, typename = void>
        struct getter_make_impl
        {
            template<typename... Args>
            static constexpr decltype(auto) apply(Args&&... args)
                { static_assert(sizeof...(args) == -1, "Invalid arguments to schema::getter::make(...)!"); }
        };

        template<typename T_dataset, typename T_value>
        struct getter_make_impl<mp::list<T_value T_dataset::*>, void>
        {
            static constexpr decltype(auto) apply(T_value T_dataset::*member)
                { return make_getter_member_var(member); }
        };

        template<typename T_dataset, typename T_value>
        struct getter_make_impl<mp::list<T_value (T_dataset::*)(void)>, void>
        {
            static constexpr decltype(auto) apply(T_value (T_dataset::*member)(void))
                { return make_getter_member_func(member); }
        };

        template<typename T_dataset, typename T_value>
        struct getter_make_impl<mp::list<T_value (T_dataset::*)(void) const>, void>
        {
            static constexpr decltype(auto) apply(T_value (T_dataset::*member)(void) const)
                { return make_getter_member_func(member); }
        };

        template<typename T_func, typename T_wrapped_dataset, typename T_value_type>
        struct getter_make_impl<mp::list<T_func, T_wrapped_dataset, T_value_type>, mp::enable_if_c<
                hana::is_a<hana::type_tag, mp::decay_t<T_wrapped_dataset>>
            &&  hana::is_a<hana::type_tag, mp::decay_t<T_value_type>>>>
        {
            static constexpr decltype(auto) apply(T_func&& func, T_wrapped_dataset&& wrapped_dataset, T_value_type&& value_type)
                { return make_getter_lambda(std::forward<T_func>(func), std::forward<T_wrapped_dataset>(wrapped_dataset), std::forward<T_value_type>(value_type)); }
        };

    }

    namespace getter
    {

        constexpr decltype(auto) make = misc::make_generic_predicate<__impl::getter_make_impl> { };

    }

}
end_namespace_cpphibernate_schema