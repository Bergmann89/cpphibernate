#pragma once

#include <memory>

#include <cpphibernate/config.h>
#include <cpphibernate/misc/meta.h>

#include <cpputils/container/nullable.h>

beg_namespace_cpphibernate_misc
{

    /* nullable_helper */

    template<typename T_nullable, typename = void>
    struct nullable_helper
    {
        using nullable_type = T_nullable;
        using value_type    = real_dataset_t<nullable_type>;

        static value_type*  get   (const nullable_type&)                = delete;
        static value_type&  set   (nullable_type&, const value_type&)   = delete;
        static void         clear (nullable_type&)                      = delete;
    };

    /* nullable_helper - utl::nullable */

    template<typename T_value>
    struct nullable_helper<utl::nullable<T_value>, void>
    {
        using nullable_type = utl::nullable<T_value>;
        using value_type    = T_value;

        static inline value_type* get(nullable_type& x)
            { return x.has_value() ? &x.value() : nullptr; }

        static inline const value_type* get(const nullable_type& x)
            { return x.has_value() ? &x.value() : nullptr; }

        static inline value_type& set(nullable_type& x, const value_type* value)
            { return *(x = *value); }

        static inline value_type& set(nullable_type& x, const value_type& value)
            { return *(x = value); }

        static inline value_type& set(nullable_type& x, value_type&& value)
            { return *(x = std::move(value)); }

        static void clear(nullable_type& x)
            { x.reset(); }
    };

    /* nullable_helper - std::unique_ptr */

    template<typename T_value>
    struct nullable_helper<std::unique_ptr<T_value>, void>
    {
        using nullable_type = std::unique_ptr<T_value>;
        using value_type    = T_value;

        static inline value_type* get(const nullable_type& x)
            { return x.get(); }

        static inline value_type& set(nullable_type& x, value_type* value)
        {
            x.reset(value);
            return *x;
        }

        static inline value_type& set(nullable_type& x, const value_type& value)
        {
            x.reset(new value_type(value));
            return *x;
        }

        static inline value_type& set(nullable_type& x, value_type&& value)
        {
            x.reset(new value_type(std::move(value)));
            return *x;
        }

        static void clear(nullable_type& x)
            { return x.reset(); }
    };

    /* nullable_helper - std::shared_ptr */

    template<typename T_value>
    struct nullable_helper<std::shared_ptr<T_value>, void>
    {
        using nullable_type = std::shared_ptr<T_value>;
        using value_type    = T_value;

        static inline value_type* get(const nullable_type& x)
            { return x.get(); }

        static inline value_type& set(nullable_type& x, value_type* value)
        {
            x.reset(value);
            return *x;
        }

        static inline value_type& set(nullable_type& x, const value_type& value)
        {
            x.reset(new value_type(value));
            return *x;
        }

        static inline value_type& set(nullable_type& x, value_type&& value)
        {
            x.reset(new value_type(std::move(value)));
            return *x;
        }

        static void clear(nullable_type& x)
            { return x.reset(); }
    };

}
end_namespace_cpphibernate_misc