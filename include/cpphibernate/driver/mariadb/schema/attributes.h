#pragma once

#include <set>

#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/schema/attributes.h>
#include <cpphibernate/driver/mariadb/schema/attributes.fwd.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* attributes_t */

    struct attributes_t :
        public std::set<attribute_t>
    {
        using base_type = std::set<attribute_t>;
        using base_type::base_type;
    };

    namespace __impl
    {

        /* attribute_converter */

        template<typename T_attribute>
        struct attribute_converter;

        template<>
        struct attribute_converter<schema::attribute::hex_type>
            { static constexpr decltype(auto) value = attribute_t::hex; };

        template<>
        struct attribute_converter<schema::attribute::compress_type>
            { static constexpr decltype(auto) value = attribute_t::compress; };

        template<>
        struct attribute_converter<schema::attribute::primary_key_type>
            { static constexpr decltype(auto) value = attribute_t::primary_key; };

        /* make_attributes_impl */

        template<typename T, typename>
        struct make_attributes_impl
        {
            template<typename... T_args>
            static constexpr decltype(auto) apply(T_args&&... args)
                { static_assert(sizeof...(args) == 0, "Invalid parameters for mariadb::make_attributes(...)!"); }
        };

        template<typename T_attributes>
        struct make_attributes_impl<
            mp::list<T_attributes>,
            mp::enable_if_c<
                schema::is_attributes<mp::decay_t<T_attributes>>::value>>
        {
            template<size_t... I>
            static constexpr decltype(auto) helper(T_attributes&&, const std::index_sequence<I...>&)
            {
                return attributes_t({
                    attribute_converter<mp::decay_t<decltype(std::declval<T_attributes>()[hana::size_c<I>])>>::value...
                });
            }

            static constexpr decltype(auto) apply(const T_attributes& attributes)
            {
                using size = mp::decay_t<decltype(hana::size(attributes))>;
                return helper(attributes, std::make_index_sequence<size::value> { });
            }
        };

    }

}
end_namespace_cpphibernate_driver_mariadb