#pragma once

#include <cpphibernate/config.h>

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

}
end_namespace_cpphibernate_misc