#pragma once

#include <iostream> // TODO debug!

#include <cpphibernate/config.h>
#include <cpputils/misc/type_helper.h>

beg_namespace_cpphibernate_misc
{

    /* make_generic_predicate */

    template<template<typename...> class T_builder>
    struct make_generic_predicate
    {
        template<typename... T_args>
        constexpr decltype(auto) operator()(T_args&&... args) const
            { return T_builder<mp::list<T_args...>>::apply(std::forward<T_args>(args)...); }
    };

    /* get_dataset_id */

    namespace __impl
    {
        struct counter_type_id
            { };
    }

    template<typename T_type>
    constexpr decltype(auto) get_type_id(T_type&&)
        { return utl::get_unique_id<__impl::counter_type_id, mp::decay_t<T_type>>(); }

}
end_namespace_cpphibernate_misc