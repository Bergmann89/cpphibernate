#pragma once

#include <cpphibernate/misc.h>
#include <cpphibernate/types.h>
#include <cpphibernate/config.h>
#include <cpputils/container/nullable.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* value_t */

    using value_t = utl::nullable<std::string>;

    /* type_properties */

    template<typename T, typename = void>
    struct type_properties
    {
        static constexpr void        type              ()               = delete;
        static           T           convert_to        (const value_t&) = delete;
        static           value_t     convert_from      (const T&)       = delete;
        static constexpr const char* convert_to_open   ()               = delete;
        static constexpr const char* convert_to_close  ()               = delete;
        static constexpr const char* convert_from_open ()               = delete;
        static constexpr const char* convert_from_close()               = delete;
    };

    template<>
    struct type_properties<bool, void>
    {
        static constexpr decltype(auto) type()
            { return "BOOLEAN"; }

        static inline bool convert_to(const value_t& value)
            { return utl::from_string<int>(*value); }

        static inline value_t convert_from(const bool& value)
            { return utl::to_string(static_cast<int>(value)); }

        static constexpr const char* convert_to_open()
            { return nullptr; }

        static constexpr const char* convert_to_close()
            { return nullptr; }

        static constexpr const char* convert_from_open()
            { return nullptr; }

        static constexpr const char* convert_from_close()
            { return nullptr; }
    };

    template<>
    struct type_properties<uint8_t, void>
    {
        static constexpr decltype(auto) type()
            { return "TINYINT UNSIGNED"; }

        static inline uint8_t convert_to(const value_t& value)
            { return static_cast<uint8_t>(utl::from_string<int>(*value)); }

        static inline value_t convert_from(const uint8_t& value)
            { return utl::to_string(static_cast<int>(value)); }

        static constexpr const char* convert_to_open()
            { return nullptr; }

        static constexpr const char* convert_to_close()
            { return nullptr; }

        static constexpr const char* convert_from_open()
            { return nullptr; }

        static constexpr const char* convert_from_close()
            { return nullptr; }
    };

    template<>
    struct type_properties<int8_t, void>
    {
        static constexpr decltype(auto) type()
            { return "TINYINT"; }

        static inline int8_t convert_to(const value_t& value)
            { return static_cast<int8_t>(utl::from_string<int>(*value)); }

        static inline value_t convert_from(const int8_t& value)
            { return utl::to_string(static_cast<int>(value)); }

        static constexpr const char* convert_to_open()
            { return nullptr; }

        static constexpr const char* convert_to_close()
            { return nullptr; }

        static constexpr const char* convert_from_open()
            { return nullptr; }

        static constexpr const char* convert_from_close()
            { return nullptr; }
    };

    template<>
    struct type_properties<uint16_t, void>
    {
        static constexpr decltype(auto) type()
            { return "SMALLINT UNSIGNED"; }

        static inline uint16_t convert_to(const value_t& value)
            { return utl::from_string<uint16_t>(*value); }

        static inline value_t convert_from(const uint16_t& value)
            { return utl::to_string(value); }

        static constexpr const char* convert_to_open()
            { return nullptr; }

        static constexpr const char* convert_to_close()
            { return nullptr; }

        static constexpr const char* convert_from_open()
            { return nullptr; }

        static constexpr const char* convert_from_close()
            { return nullptr; }
    };

    template<>
    struct type_properties<int16_t, void>
    {
        static constexpr decltype(auto) type()
            { return "SMALLINT"; }

        static inline int16_t convert_to(const value_t& value)
            { return utl::from_string<int16_t>(*value); }

        static inline value_t convert_from(const int16_t& value)
            { return utl::to_string(value); }

        static constexpr const char* convert_to_open()
            { return nullptr; }

        static constexpr const char* convert_to_close()
            { return nullptr; }

        static constexpr const char* convert_from_open()
            { return nullptr; }

        static constexpr const char* convert_from_close()
            { return nullptr; }
    };

    template<>
    struct type_properties<uint32_t, void>
    {
        static constexpr decltype(auto) type()
            { return "INT UNSIGNED"; }

        static inline uint32_t convert_to(const value_t& value)
            { return utl::from_string<uint32_t>(*value); }

        static inline value_t convert_from(const uint32_t& value)
            { return utl::to_string(value); }

        static constexpr const char* convert_to_open()
            { return nullptr; }

        static constexpr const char* convert_to_close()
            { return nullptr; }

        static constexpr const char* convert_from_open()
            { return nullptr; }

        static constexpr const char* convert_from_close()
            { return nullptr; }
    };

    template<>
    struct type_properties<int32_t, void>
    {
        static constexpr decltype(auto) type()
            { return "INT"; }

        static inline int32_t convert_to(const value_t& value)
            { return utl::from_string<int32_t>(*value); }

        static inline value_t convert_from(const int32_t& value)
            { return utl::to_string(value); }

        static constexpr const char* convert_to_open()
            { return nullptr; }

        static constexpr const char* convert_to_close()
            { return nullptr; }

        static constexpr const char* convert_from_open()
            { return nullptr; }

        static constexpr const char* convert_from_close()
            { return nullptr; }
    };

    template<>
    struct type_properties<uint64_t, void>
    {
        static constexpr decltype(auto) type()
            { return "BIGINT UNSIGNED"; }

        static inline uint64_t convert_to(const value_t& value)
            { return utl::from_string<uint64_t>(*value); }

        static inline value_t convert_from(const uint64_t& value)
            { return utl::to_string(value); }

        static constexpr const char* convert_to_open()
            { return nullptr; }

        static constexpr const char* convert_to_close()
            { return nullptr; }

        static constexpr const char* convert_from_open()
            { return nullptr; }

        static constexpr const char* convert_from_close()
            { return nullptr; }
    };

    template<>
    struct type_properties<int64_t, void>
    {
        static constexpr decltype(auto) type()
            { return "BIGINT"; }

        static inline int64_t convert_to(const value_t& value)
            { return utl::from_string<int64_t>(*value); }

        static inline value_t convert_from(const int64_t& value)
            { return utl::to_string(value); }

        static constexpr const char* convert_to_open()
            { return nullptr; }

        static constexpr const char* convert_to_close()
            { return nullptr; }

        static constexpr const char* convert_from_open()
            { return nullptr; }

        static constexpr const char* convert_from_close()
            { return nullptr; }
    };

    template<>
    struct type_properties<float, void>
    {
        static constexpr decltype(auto) type()
            { return "FLOAT"; }

        static inline float convert_to(const value_t& value)
            { return utl::from_string<float>(*value); }

        static inline value_t convert_from(const float& value)
            { return utl::to_string(value); }

        static constexpr const char* convert_to_open()
            { return nullptr; }

        static constexpr const char* convert_to_close()
            { return nullptr; }

        static constexpr const char* convert_from_open()
            { return nullptr; }

        static constexpr const char* convert_from_close()
            { return nullptr; }
    };

    template<>
    struct type_properties<double, void>
    {
        static constexpr decltype(auto) type()
            { return "DOUBLE"; }

        static inline double convert_to(const value_t& value)
            { return utl::from_string<double>(*value); }

        static inline value_t convert_from(const double& value)
            { return utl::to_string(value); }

        static constexpr const char* convert_to_open()
            { return nullptr; }

        static constexpr const char* convert_to_close()
            { return nullptr; }

        static constexpr const char* convert_from_open()
            { return nullptr; }

        static constexpr const char* convert_from_close()
            { return nullptr; }
    };

    template<>
    struct type_properties<uuid, void>
    {
        static constexpr decltype(auto) type()
            { return "BINARY(16)"; }

        static inline uuid convert_to(const value_t& value)
            { return utl::from_string<uuid>(*value); }

        static inline value_t convert_from(const uuid& value)
            { return utl::to_string(value); }

        static constexpr const char* convert_to_open()
            { return nullptr; }

        static constexpr const char* convert_to_close()
            { return nullptr; }

        static constexpr const char* convert_from_open()
            { return nullptr; }

        static constexpr const char* convert_from_close()
            { return nullptr; }
    };

    template<>
    struct type_properties<timestamp, void>
    {
        static constexpr decltype(auto) type()
            { return "DATETIME"; }

        static inline timestamp convert_to(const value_t& value)
            { return timestamp(utl::from_string<uint64_t>(*value)); }

        static inline value_t convert_from(const timestamp& value)
            { return utl::to_string(static_cast<uint64_t>(value)); }

        static constexpr const char* convert_to_open()
            { return "FROM_UNIXTIME("; }

        static constexpr const char* convert_to_close()
            { return ")"; }

        static constexpr const char* convert_from_open()
            { return "UNIX_TIMESTAMP("; }

        static constexpr const char* convert_from_close()
            { return ")"; }
    };

    template<>
    struct type_properties<std::string, void>
    {
        static constexpr decltype(auto) type()
            { return "VARCHAR(100)"; }

        static inline std::string convert_to(const value_t& value)
            { return *value; }

        static inline value_t convert_from(const std::string& value)
            { return value; }

        static constexpr const char* convert_to_open()
            { return nullptr; }

        static constexpr const char* convert_to_close()
            { return nullptr; }

        static constexpr const char* convert_from_open()
            { return nullptr; }

        static constexpr const char* convert_from_close()
            { return nullptr; }
    };

    template<size_t N>
    struct type_properties<string<N>, void>
    {
        static inline std::string make_type()
            { return std::string("VARCHAR(") + utl::to_string(N) + ")"; }

        static inline decltype(auto) type()
        {
            static const std::string v = make_type();
            return v;
        }

        static inline std::string convert_to(const value_t& value)
            { return *value; }

        static inline value_t convert_from(const std::string& value)
            { return value; }

        static constexpr const char* convert_to_open()
            { return nullptr; }

        static constexpr const char* convert_to_close()
            { return nullptr; }

        static constexpr const char* convert_from_open()
            { return nullptr; }

        static constexpr const char* convert_from_close()
            { return nullptr; }
    };

    template<typename T>
    struct type_properties<T, mp::enable_if<misc::is_nullable<mp::decay_t<T>>>>
    {
        using nullable_type         = T;
        using nullable_helper_type  = misc::nullable_helper<nullable_type>;
        using value_type            = typename nullable_helper_type::value_type;
        using value_type_props      = type_properties<value_type>;

        static constexpr decltype(auto) type()
            { return value_type_props::type(); }

        static inline nullable_type convert_to(const value_t& value)
        {
            nullable_type ret;
            if (value.has_value())
                nullable_helper_type::set(ret, value_type_props::convert_to(value));
            return ret;
        }

        static inline value_t convert_from(const nullable_type& value)
        {
            value_t ret;
            auto v = nullable_helper_type::get(value);
            if (v)
                ret = value_type_props::convert_from(*v);
            return ret;
        }

        static constexpr const char* convert_to_open()
            { return nullptr; }

        static constexpr const char* convert_to_close()
            { return nullptr; }

        static constexpr const char* convert_from_open()
            { return nullptr; }

        static constexpr const char* convert_from_close()
            { return nullptr; }
    };

    template<typename T>
    struct type_properties<T, mp::enable_if<std::is_enum<mp::clean_type<T>>>>
    {
        using enum_type = T;
        using base_type = typename std::underlying_type<enum_type>::type;

        static std::string make_type()
        {
            std::ostringstream os;
            os << "ENUM ( ";
            auto e = enum_type::first;
            while (e <= enum_type::last)
            {
                if (e != enum_type::first)
                    os << ", ";
                os << "'" << utl::enum_conversion<enum_type>::to_string(e, false) << "'";
                e = static_cast<enum_type>(static_cast<base_type>(e) + 1);
            }
            os << " )";
            return os.str();
        }

        static inline decltype(auto) type()
        {
            static const std::string v = make_type();
            return v;
        }

        static inline enum_type convert_to(const value_t& value)
        {
            enum_type ret;
            if (!utl::enum_conversion<enum_type>::try_to_enum(*value, ret, false))
                throw misc::hibernate_exception("unable to convert enum value!");
            return ret;
        }

        static inline value_t convert_from(const enum_type& value)
            { return utl::enum_conversion<enum_type>::to_string(value, false); }

        static constexpr const char* convert_to_open()
            { return nullptr; }

        static constexpr const char* convert_to_close()
            { return nullptr; }

        static constexpr const char* convert_from_open()
            { return nullptr; }

        static constexpr const char* convert_from_close()
            { return nullptr; }
    };

    template<typename T>
    struct type_properties<T, mp::enable_if<mp::is_specialization_of<mp::clean_type<T>, utl::flags>>>
    {
        using flags_type    = T;
        using enum_type     = typename flags_type::enum_type;
        using base_type     = typename std::underlying_type<enum_type>::type;

        static inline std::string make_type()
        {
            std::ostringstream os;
            os << "SET ( ";
            auto e = enum_type::first;
            while (e <= enum_type::last)
            {
                if (e != enum_type::first)
                    os << ", ";
                os << "'" << utl::to_string(e) << "'";
                e = static_cast<enum_type>(static_cast<base_type>(e) + 1);
            }
            os << " )";
            return os.str();
        }

        static inline decltype(auto) type()
        {
            static const std::string v = make_type();
            return v;
        }

        static inline flags_type convert_to(const value_t& value)
        {
            auto s = *value;
            auto c = s.c_str();
            auto e = c + s.size();
            auto p = c;
            flags_type ret;
            while (c <= e)
            {
                if (c == e || *c == ',')
                {
                    if (c - p > 0)
                        ret.set(utl::enum_conversion<enum_type>::to_enum(std::string(p, static_cast<size_t>(c - p)), true));
                    p = c + 1;
                }
                ++c;
            }
            return ret;
        }

        static inline value_t convert_from(const flags_type& value)
        {
            std::ostringstream os;
            bool first = true;
            for (auto e : value)
            {
                if (first)  first = false;
                else        os << ",";
                utl::to_string(os, static_cast<int>(e));
            }
            return os.str();
        }

        static constexpr const char* convert_to_open()
            { return nullptr; }

        static constexpr const char* convert_to_close()
            { return nullptr; }

        static constexpr const char* convert_from_open()
            { return nullptr; }

        static constexpr const char* convert_from_close()
            { return nullptr; }
    };

}
end_namespace_cpphibernate_driver_mariadb