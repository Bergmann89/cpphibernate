#pragma once

#include <boost/hana.hpp>
#include <cpputils/mp/core.h>

#define cpphibernate_equality_comparable()                                          \
    template<typename T_other>                                                      \
    constexpr decltype(auto) operator==(T_other&&) const                            \
    {                                                                               \
        return ::boost::hana::type<::utl::mp::decay_t<decltype(*this)>> { } ==      \
               ::boost::hana::type<::utl::mp::decay_t<T_other>> { };                \
    }                                                                               \
                                                                                    \
    template<typename T_other>                                                      \
    constexpr decltype(auto) operator!=(T_other&&) const                            \
    {                                                                               \
        return ::boost::hana::type<::utl::mp::decay_t<decltype(*this)>> { } !=      \
               ::boost::hana::type<::utl::mp::decay_t<T_other>> { };                \
    }

#define cpphibernate_constructable(name, value)                                     \
    name() = value

#define cpphibernate_copyable(name, value)                                          \
    name(const name&) = value;                                                      \
    name& operator=(const name&) = value

#define cpphibernate_moveable(name, value)                                          \
    name(name&&) = value;                                                           \
    name& operator=(name&&) = value

#define cpphibernate_define_namespace_beg(parent, name)     \
    parent {                                                \
    namespace name

#define cpphibernate_define_namespace_end(parent)           \
    }                                                       \
    parent

#define beg_namespace_cpphibernate              namespace cpphibernate
#define end_namespace_cpphibernate

#define beg_namespace_cpphibernate_schema       cpphibernate_define_namespace_beg(beg_namespace_cpphibernate, schema)
#define end_namespace_cpphibernate_schema       cpphibernate_define_namespace_end(end_namespace_cpphibernate)

#define beg_namespace_cpphibernate_misc         cpphibernate_define_namespace_beg(beg_namespace_cpphibernate, misc)
#define end_namespace_cpphibernate_misc         cpphibernate_define_namespace_end(end_namespace_cpphibernate)

beg_namespace_cpphibernate
{

    namespace mp   = ::utl::mp;
    namespace hana = ::boost::hana;

}
end_namespace_cpphibernate
