#pragma once

#include <list>
#include <vector>
#include <memory>

#include <cpphibernate/config.h>

#include <cpputils/container/nullable.h>

beg_namespace_cpphibernate_misc
{

    namespace __impl
    {

        /* is_container_impl */

        template<typename T>
        struct is_container_impl
            : public mp::c_false_t
            { };

        template<typename T>
        struct is_container_impl<std::list<T>>
            : public mp::c_true_t
            { };

        template<typename T>
        struct is_container_impl<std::vector<T>>
            : public mp::c_true_t
            { };

        /* is_nullable_impl */

        template<typename T>
        struct is_nullable_impl
            : public mp::c_false_t
            { };

        template<typename T>
        struct is_nullable_impl<utl::nullable<T>>
            : public mp::c_true_t
            { };

        template<typename T>
        struct is_nullable_impl<std::unique_ptr<T>>
            : public mp::c_true_t
            { };

        template<typename T>
        struct is_nullable_impl<std::shared_ptr<T>>
            : public mp::c_true_t
            { };

        /* is_pointer_impl */

        template<typename T>
        struct is_pointer_impl
            : public mp::c_false_t
            { };

        template<typename T>
        struct is_pointer_impl<std::unique_ptr<T>>
            : public mp::c_true_t
            { };

        template<typename T>
        struct is_pointer_impl<std::shared_ptr<T>>
            : public mp::c_true_t
            { };

        /* real_dataset_impl */

        template<typename T, typename = void>
        struct real_dataset_impl
            { using type = T; };

        template<typename T>
        struct real_dataset_impl<utl::nullable<T>, void>
            { using type = typename real_dataset_impl<T>::type; };

        template<typename T>
        struct real_dataset_impl<std::unique_ptr<T>, void>
            { using type = typename real_dataset_impl<T>::type; };

        template<typename T>
        struct real_dataset_impl<std::shared_ptr<T>, void>
            { using type = typename real_dataset_impl<T>::type; };

        template<typename T>
        struct real_dataset_impl<std::vector<T>, void>
            { using type = typename real_dataset_impl<T>::type; };

        template<typename T>
        struct real_dataset_impl<std::list<T>, void>
            { using type = typename real_dataset_impl<T>::type; };

        template<typename T_from, typename T_to, typename = void>
        struct change_dataset_type_impl
            { using type = T_to; };

        template<template<typename...> class X, typename T_from, typename T_to>
        struct change_dataset_type_impl<X<T_from>, T_to, void>
            { using type = X<T_to>; };

    }

    /* meta */

    template<typename T>
    struct is_container
        : public __impl::is_container_impl<T>
        { };

    template<typename T>
    struct is_nullable
        : public __impl::is_nullable_impl<T>
        { };

    template<typename T>
    struct is_pointer
        : public __impl::is_pointer_impl<T>
        { };

    template<typename T>
    using real_dataset_t = typename __impl::real_dataset_impl<T>::type;

}
end_namespace_cpphibernate_misc