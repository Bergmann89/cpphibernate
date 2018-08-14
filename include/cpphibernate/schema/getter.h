#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/misc/general.h>

beg_namespace_cpphibernate_schema
{

    namespace __impl
    {

        /* getter_t */

        template<typename T_value>
        struct getter_t
        {
            using value_type = T_value;
        };


        /* getter_none_t */

        struct getter_none_t
            : public getter_t<void>
        {
            using base_type  = getter_t<void>;
            using value_type = typename base_type::value_type;

            cpphibernate_constructable(getter_none_t, default);
            cpphibernate_copyable     (getter_none_t, delete);
            cpphibernate_moveable     (getter_none_t, default);
        };

        /* getter_member_var_t */

        template<typename T_dataset, typename T_value, typename T_member>
        struct getter_member_var_t
            : public getter_t<T_value>
        {
            using base_type     = getter_t<T_value>;
            using value_type    = typename base_type::value_type;
            using dataset_type  = T_dataset;
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
            : public getter_t<T_value>
        {
            using base_type     = getter_t<T_value>;
            using value_type    = typename base_type::value_type;
            using dataset_type  = T_dataset;
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
            : public getter_t<decltype(std::declval<T_lambda>()(std::declval<T_dataset>()))>
        {
            using base_type     = getter_t<decltype(std::declval<T_lambda>()(std::declval<T_dataset>()))>;
            using value_type    = typename base_type::value_type;
            using dataset_type  = T_dataset;
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
                mp::is_base_of<getter_t<typename T::value_type>, T>::value>>
            : public mp::c_true_t
            { };

    }

    /* meta */

    template<typename T>
    struct is_getter :
        public __impl::is_getter_impl<T>
        { };

    /* make */

    constexpr decltype(auto) make_getter_none()
        { return __impl::getter_none_t(); }

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


    namespace __impl
    {

        /* getter_buildser */

        template<typename X, typename = void>
        struct getter_builder
        {
            template<typename... Args>
            static constexpr decltype(auto) apply(Args&&...)
                { return make_getter_none(); }
        };

        template<typename T_dataset, typename T_value>
        struct getter_builder<mp::list<T_value T_dataset::*>, void>
        {
            static constexpr decltype(auto) apply(T_value T_dataset::*member)
                { return make_getter_member_var(member); }
        };

        template<typename T_dataset, typename T_value>
        struct getter_builder<mp::list<T_value (T_dataset::*)(void)>, void>
        {
            static constexpr decltype(auto) apply(T_value (T_dataset::*member)(void))
                { return make_getter_member_func(member); }
        };

        template<typename T_dataset, typename T_value>
        struct getter_builder<mp::list<T_value (T_dataset::*)(void) const>, void>
        {
            static constexpr decltype(auto) apply(T_value (T_dataset::*member)(void) const)
                { return make_getter_member_func(member); }
        };

        template<typename T_func, typename T_dataset_type, typename T_value_type>
        struct getter_builder<mp::list<T_func, T_dataset_type, T_value_type>, mp::enable_if_c<
                hana::is_a<hana::type_tag, mp::decay_t<T_dataset_type>>
            &&  hana::is_a<hana::type_tag, mp::decay_t<T_value_type>>>>
        {
            static constexpr decltype(auto) apply(T_func&& func, T_dataset_type&& dataset_type, T_value_type&& value_type)
                { return make_getter_lambda(std::forward<T_func>(func), std::forward<T_dataset_type>(dataset_type), std::forward<T_value_type>(value_type)); }
        };

    }

    /* make */

    constexpr decltype(auto) make_getter = misc::make_generic_predicate<__impl::getter_builder> { };

}
end_namespace_cpphibernate_schema