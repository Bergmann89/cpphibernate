#pragma once

#include <cpphibernate/types.h>
#include <cpphibernate/config.h>
#include <cpputils/container/nullable.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* key_properties */

    template<typename T_key, typename = void>
    struct key_properties;

    template<>
    struct key_properties<uuid, void>
    {
        using auto_generated    = mp::c_false_t;
        using key_type          = uuid;

        static constexpr decltype(auto) create_table_argument = "";
        static constexpr decltype(auto) create_key_query      = "SELECT Uuid()";
        static constexpr decltype(auto) convert_to_open       = "UuidToBin(";
        static constexpr decltype(auto) convert_to_close      = ")";
        static constexpr decltype(auto) convert_from_open     = "BinToUuid(";
        static constexpr decltype(auto) convert_from_close    = ")";

        static bool is_default(const key_type& key)
            { return key == key_type(); }
    };

    template<typename T_key>
    struct key_properties<T_key, mp::enable_if<mp::is_integral<T_key>>>
    {
        using auto_generated    = mp::c_true_t;
        using key_type          = T_key;

        static constexpr decltype(auto) create_table_argument = "AUTO_INCREMENT";
        static constexpr decltype(auto) create_key_query      = "";
        static constexpr decltype(auto) convert_to_open       = "";
        static constexpr decltype(auto) convert_to_close      = "";
        static constexpr decltype(auto) convert_from_open     = "";
        static constexpr decltype(auto) convert_from_close    = "";

        static bool is_default(const key_type& key)
            { return key == key_type(); }
    };

}
end_namespace_cpphibernate_driver_mariadb