#pragma once

#include <set>
#include <list>
#include <vector>

#include <cpphibernate/config.h>

beg_namespace_cpphibernate_misc
{

    /* container_helper */

    template<typename T_container, typename = void>
    struct container_helper;

    template<typename T_value, typename... T_args>
    struct container_helper<std::vector<T_value, T_args...>, void>
    {
        using container_type = std::vector<T_value, T_args...>;
        using value_type     = T_value;

        template<typename... T_xargs>
        static inline value_type& emplace(container_type& container, T_xargs&&... args)
        {
            container.emplace_back(std::forward<T_xargs>(args)...);
            return container.back();
        }

        static inline void clear(container_type& container)
            { container.clear(); }
    };

    template<typename T_value, typename... T_args>
    struct container_helper<std::list<T_value, T_args...>, void>
    {
        using container_type = std::list<T_value, T_args...>;
        using value_type     = T_value;

        template<typename... T_xargs>
        static inline value_type& emplace(container_type& container, T_xargs&&... args)
        {
            container.emplace_back(std::forward<T_xargs>(args)...);
            return container.back();
        }

        static inline void clear(container_type& container)
            { container.clear(); }
    };

}
end_namespace_cpphibernate_misc