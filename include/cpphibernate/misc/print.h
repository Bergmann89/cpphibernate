#pragma once

#include <list>
#include <vector>
#include <memory>
#include <iostream>

#include <cpphibernate/config.h>
#include <cpputils/misc/indent.h>

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

beg_namespace_cpphibernate_misc
{

    template<typename T_container, typename T_func>
    struct container_printer
    {
        T_container container;
        bool        do_indent;
        T_func      func;

        inline void print(std::ostream& os) const
        {
            using namespace ::utl;
            auto beg = std::begin(container);
            auto end = std::end  (container);
            if (beg != end)
            {
                if (do_indent)
                {
                    os  << indent << "["
                        << incindent;
                }
                else
                {
                    os << "[";
                }

                size_t index = 0;
                for (auto it = beg; it != end; ++it)
                {
                    if (index++)
                        os << ",";
                    if (!do_indent)
                        os << " ";
                    func(os, *it);
                }

                if (do_indent)
                {
                    os  << decindent
                        << indent << "]";
                }
                else
                {
                    os << " ]";
                }
            }
            else
            {
                os << "[ ]";
            }
        }
    };

    struct print_container_builder
    {
        template<typename T_container, typename T_func>
        constexpr decltype(auto) operator()(T_container&& container, bool do_indent, T_func&& func) const
        {
            return container_printer<T_container, T_func>
            {
                std::forward<T_container>(container),
                do_indent,
                std::forward<T_func>(func)
            };
        }

        template<typename T_container>
        constexpr decltype(auto) operator()(T_container&& container, bool do_indent) const
        {
            return this->operator()(
                std::forward<T_container>(container),
                do_indent,
                [do_indent](auto& os, auto& value)
                {
                    using namespace ::utl;
                    if (do_indent)
                        os << indent;
                    os << utl::to_string(value);
                });
        }
    };

    constexpr decltype(auto) print_container = print_container_builder { };

}
end_namespace_cpphibernate_misc