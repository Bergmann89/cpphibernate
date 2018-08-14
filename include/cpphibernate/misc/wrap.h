#pragma once

#include <list>
#include <vector>
#include <memory>

#include <cpphibernate/config.h>

#include <cpputils/container/nullable.h>

beg_namespace_cpphibernate_misc
{

    template<typename T>
    using unwrap_t = typename T::type;

    template<typename T>
    using decay_unwrap_t = typename mp::decay_t<T>::type;

    namespace __impl
    {

        struct wrap_t
        {
            template<typename T>
            constexpr decltype(auto) operator()(T) const noexcept
                { return hana::type_c<T>; }
        };

        struct unwrapped_t
        {
            template<typename T>
            constexpr decltype(auto) operator()(T) const noexcept
                { return unwrap_t<T> { }; }
        };

    }

    constexpr decltype(auto) wrap = __impl::wrap_t { };

    constexpr decltype(auto) unwrap = __impl::unwrapped_t { };

}
end_namespace_cpphibernate_misc