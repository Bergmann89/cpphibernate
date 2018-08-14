#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/types.h>

#define cpphibernate_make_string(str)                                   \
    #str

#define cpphibernate_make_schema(name, ...)                             \
    cpphibernate::schema::make_schema(                                  \
        cpphibernate_make_string(name),                                 \
        cpphibernate::schema::make_tables(__VA_ARGS__))

#define cpphibernate_make_table_name(name, type, id, ...)               \
    cpphibernate::schema::make_table(                                   \
        cpphibernate_make_string(name),                                 \
        boost::hana::type_c<type>,                                      \
        boost::hana::size_c<id>,                                        \
        cpphibernate::schema::make_fields(__VA_ARGS__))

#define cpphibernate_make_table(name, id, ...)                          \
    cpphibernate_make_table_name(name, name, id, __VA_ARGS__)

#define cpphibernate_make_field_custom(name, getter, setter, ...)       \
    cpphibernate::schema::make_field(                                   \
        cpphibernate_make_string(name),                                 \
        cpphibernate::schema::make_getter(getter),                      \
        cpphibernate::schema::make_setter(setter),                      \
        cpphibernate::schema::make_attributes(__VA_ARGS__))

#define cpphibernate_make_field_name(name, member_ptr, ...)             \
    cpphibernate_make_field_custom(                                     \
        name, member_ptr, member_ptr, __VA_ARGS__)

#define cpphibernate_make_field(type, member, ...)                      \
    cpphibernate_make_field_name(                                       \
        member, &type::member, __VA_ARGS__)

#define cpphibernate_make_id(member_ptr)                                \
    cpphibernate_make_field_name(                                       \
        null,                                                           \
        member_ptr,                                                     \
        cpphibernate::schema::attribute::primary_key)