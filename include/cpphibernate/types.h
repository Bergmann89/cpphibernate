#pragma once

#include <string>
#include <iostream>

#include <cpphibernate/config.h>

#include <cpputils/misc/exception.h>

beg_namespace_cpphibernate
{

    /* string */

    template<size_t N>
    struct string
        : public std::string
    {
        static constexpr decltype(auto) max_size = N;
        using std::string::string;
        using std::string::operator=;
    };

    /* timestamp */

    struct timestamp
    {
        uint64_t value;

        inline timestamp() = default;

        inline timestamp(uint64_t v)
            : value(v)
            { }

        inline timestamp& operator=(const uint64_t& v)
        {
            value = v;
            return *this;
        }

        inline operator uint64_t() const
            { return value; }
    };

    /* uuid */

    struct uuid
        : public std::array<uint8_t, 16>
    {
    public:
        inline uuid()
            : std::array<uint8_t, 16>::array({ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } })
            { }

        inline uuid(const std::string& str)
            : std::array<uint8_t, 16>::array({ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } })
        {
            if (!from_string(str, *this))
                throw utl::argument_exception("str", "invalid uuid");
        }

        cpphibernate_copyable(uuid, default);
        cpphibernate_moveable(uuid, default);

        void to_string(std::ostream& os) const;

    public:
        static bool from_string(const std::string& str, uuid& val);
    };

}
end_namespace_cpphibernate