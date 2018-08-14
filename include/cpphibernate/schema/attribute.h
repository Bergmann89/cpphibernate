#pragma once

#include <cpphibernate/config.h>

beg_namespace_cpphibernate_schema
{

    namespace __impl
    {

        /* attribute_t */

        template<typename T_inner>
        struct attribute_t
            : public T_inner
        {
            using inner_type = T_inner;
            using this_type  = attribute_t<inner_type>;

            cpphibernate_equality_comparable();
        };

        struct hex_t         { static constexpr decltype(auto) name = "hex"; };
        struct compress_t    { static constexpr decltype(auto) name = "compress"; };
        struct primary_key_t { static constexpr decltype(auto) name = "primary_key"; };

    }

    namespace attribute
    {

        using hex_type                  = __impl::attribute_t<__impl::hex_t>;
        using compress_type             = __impl::attribute_t<__impl::compress_t>;
        using primary_key_type          = __impl::attribute_t<__impl::primary_key_t>;

        /** value is stored as hexadecimal string, and will be converted to its binary form on read */
        constexpr hex_type                  hex                     { };

        /** value is stored as compressed binary, and will be uncompressed on read */
        constexpr compress_type             compress                { };

        /** this value represents a primary key (it must be stored in the dataset to be able to do the operations on the database) */
        constexpr primary_key_type          primary_key             { };

    }

    /* meta */

    template<typename T>
    struct is_attribute
        : public mp::is_specialization_of<T, __impl::attribute_t>
        { };

    template<typename... T>
    struct all_are_attribures
        : public mp::all_true<is_attribute<T>::value...>
        { };

}
end_namespace_cpphibernate_schema