#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/misc/general.h>

beg_namespace_cpphibernate_schema
{

    namespace __impl
    {

        /* setter_t */

        template<typename T_value>
        struct setter_t
        {
            using value_type = T_value;
        };


        /* setter_none_t */

        struct setter_none_t
            : public setter_t<void>
        {
            using base_type  = setter_t<void>;
            using value_type = typename base_type::value_type;

            cpphibernate_constructable(setter_none_t, default);
            cpphibernate_copyable     (setter_none_t, delete);
            cpphibernate_moveable     (setter_none_t, default);
        };

        /* setter_member_var_t */

        template<typename T_dataset, typename T_value, typename T_member>
        struct setter_member_var_t
            : public setter_t<T_value>
        {
            using base_type     = setter_t<T_value>;
            using value_type    = typename base_type::value_type;
            using dataset_type  = T_dataset;
            using member_type   = T_member;

            member_type member;

            template<typename X_member>
            constexpr setter_member_var_t(X_member&& p_member)
                : member(std::forward<X_member>(p_member))
                { }

            cpphibernate_copyable(setter_member_var_t, delete);
            cpphibernate_moveable(setter_member_var_t, default);

            template<typename X_dataset, typename X_value>
            constexpr decltype(auto) operator()(X_dataset&& data, X_value&& value) const
                { return std::forward<X_dataset>(data).*member = std::forward<X_value>(value); }
        };

        /* setter_member_func_t */

        template<typename T_dataset, typename T_value, typename T_member>
        struct setter_member_func_t
            : public setter_t<T_value>
        {
            using base_type     = setter_t<T_value>;
            using value_type    = typename base_type::value_type;
            using dataset_type  = T_dataset;
            using member_type   = T_member;

            member_type member;

            template<typename X_member>
            constexpr setter_member_func_t(X_member&& p_member)
                : member(std::forward<X_member>(p_member))
                { };

            cpphibernate_copyable(setter_member_func_t, delete);
            cpphibernate_moveable(setter_member_func_t, default);

            template<typename X_dataset, typename X_value>
            constexpr decltype(auto) operator()(X_dataset&& data, X_value&& value) const
                { return (std::forward<X_dataset>(data).*member)(std::forward<X_value>(value)); }
        };

        /* setter_lambda_t */

        template<typename T_dataset, typename T_value, typename T_lambda>
        struct setter_lambda_t
            : public setter_t<T_value>
        {
            using base_type     = setter_t<T_value>;
            using value_type    = typename base_type::value_type;
            using dataset_type  = T_dataset;
            using lambda_type   = T_lambda;

            lambda_type lambda;

            template<typename X_lambda>
            constexpr setter_lambda_t(X_lambda&& p_lambda)
                : lambda(std::forward<X_lambda>(p_lambda))
                { }

            cpphibernate_copyable(setter_lambda_t, delete);
            cpphibernate_moveable(setter_lambda_t, default);

            template<typename X_dataset, typename X_value>
            constexpr decltype(auto) operator()(X_dataset&& data, X_value&& value) const
                { return lambda(std::forward<X_dataset>(data), std::forward<X_value>(value)); }
        };

        /* is_setter_impl */

        template<typename T, typename = void>
        struct is_setter_impl
            : public mp::c_false_t
            { };

        template<typename T>
        struct is_setter_impl<T, mp::enable_if_c<
                mp::is_base_of<setter_t<typename T::value_type>, T>::value>>
            : public mp::c_true_t
            { };

    }

    /* meta */

    template<typename T>
    struct is_setter :
        public __impl::is_setter_impl<T>
        { };

    /* make */

    constexpr decltype(auto) make_setter_none()
        { return __impl::setter_none_t(); }

    template<typename T_dataset, typename T_value>
    constexpr decltype(auto) make_setter_member_var(T_value T_dataset::* member)
        { return __impl::setter_member_var_t<T_dataset, T_value, T_value T_dataset::*>(member); }

    template<typename T_dataset, typename T_value, typename T_return>
    constexpr decltype(auto) make_setter_member_func(T_return (T_dataset::*member)(T_value))
        { return __impl::setter_member_func_t<T_dataset, T_value, T_return (T_dataset::*)(T_value)>(member); }

    template<typename T_dataset, typename T_value, typename T_return>
    constexpr decltype(auto) make_setter_member_func(T_return (T_dataset::*member)(T_value) const)
        { return __impl::setter_member_func_t<const T_dataset, T_value, T_return (T_dataset::*)(T_value) const>(member); }

    template<typename T_dataset, typename T_value, typename T_lambda>
    constexpr decltype(auto) make_setter_lambda(T_lambda&& lambda, boost::hana::basic_type<T_dataset>, boost::hana::basic_type<T_value>)
        { return __impl::setter_lambda_t<T_dataset, T_value, T_lambda>(std::forward<T_lambda>(lambda)); }

    /* operations */

    namespace __impl
    {

        /* setter_make_impl */

        template<typename X, typename = void>
        struct setter_make_impl
        {
            template<typename... Args>
            static constexpr decltype(auto) apply(Args&&...)
                { return make_setter_none(); }
        };

        template<typename T_dataset, typename T_value>
        struct setter_make_impl<mp::list<T_value T_dataset::*>, void>
        {
            static constexpr decltype(auto) apply(T_value T_dataset::*member)
                { return make_setter_member_var(member); }
        };

        template<typename T_dataset, typename T_value>
        struct setter_make_impl<mp::list<T_value (T_dataset::*)(void)>, void>
        {
            static constexpr decltype(auto) apply(T_value (T_dataset::*member)(void))
                { return make_setter_member_func(member); }
        };

        template<typename T_dataset, typename T_value>
        struct setter_make_impl<mp::list<T_value (T_dataset::*)(void) const>, void>
        {
            static constexpr decltype(auto) apply(T_value (T_dataset::*member)(void) const)
                { return make_setter_member_func(member); }
        };

        template<typename T_func, typename T_wrapped_dataset, typename T_value_type>
        struct setter_make_impl<mp::list<T_func, T_wrapped_dataset, T_value_type>, mp::enable_if_c<
                hana::is_a<hana::type_tag, mp::decay_t<T_wrapped_dataset>>
            &&  hana::is_a<hana::type_tag, mp::decay_t<T_value_type>>>>
        {
            static constexpr decltype(auto) apply(T_func&& func, T_wrapped_dataset&& wrapped_dataset, T_value_type&& value_type)
                { return make_setter_lambda(std::forward<T_func>(func), std::forward<T_wrapped_dataset>(wrapped_dataset), std::forward<T_value_type>(value_type)); }
        };

    }

    namespace setter
    {

        constexpr decltype(auto) make = misc::make_generic_predicate<__impl::setter_make_impl> { };

    }

}
end_namespace_cpphibernate_schema