#pragma once

#include <cpphibernate/config.h>

namespace std
{

    template<typename T_char, typename T_traits, typename X>
    inline auto operator <<(basic_ostream<T_char, T_traits>& os, X&& x)
        -> ::utl::mp::enable_if<
                utl::mp::is_valid<decltype(std::forward<X>(x).print(os))>,
                basic_ostream<T_char, T_traits>&>
    {
        std::forward<X>(x).print(os);
        return os;
    }

}