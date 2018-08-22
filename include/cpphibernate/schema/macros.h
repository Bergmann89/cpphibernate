#pragma once

#include <cpphibernate/config.h>
#include <cpphibernate/types.h>

#define cpphibernate_make_string(str)                                   \
    #str

#define cpphibernate_make_schema(p_name, ...)                           \
    cpphibernate::schema::make(                                         \
        cpphibernate_make_string(p_name),                               \
        cpphibernate::schema::tables::make(__VA_ARGS__))

#define cpphibernate_make_table_name(p_name, p_type, p_id, ...)         \
    cpphibernate::schema::table::make(                                  \
        cpphibernate_make_string(p_name),                               \
        boost::hana::type_c<p_type>,                                    \
        boost::hana::size_c<p_id>,                                      \
        cpphibernate::schema::fields::make(__VA_ARGS__))

#define cpphibernate_make_table(p_name, p_id, ...)                      \
    cpphibernate_make_table_name(p_name, p_name, p_id, __VA_ARGS__)

#define cpphibernate_make_field_custom(p_name, p_getter, p_setter, ...) \
    cpphibernate::schema::field::make(                                  \
        cpphibernate_make_string(p_name),                               \
        cpphibernate::schema::getter::make(p_getter),                   \
        cpphibernate::schema::setter::make(p_setter),                   \
        cpphibernate::schema::attributes::make(__VA_ARGS__))

#define cpphibernate_make_field_name(p_name, p_member_ptr, ...)         \
    cpphibernate_make_field_custom(                                     \
        p_name, p_member_ptr, p_member_ptr, __VA_ARGS__)

#define cpphibernate_make_field(p_type, p_member, ...)                  \
    cpphibernate_make_field_name(                                       \
        p_member, &p_type::p_member, __VA_ARGS__)

#define cpphibernate_make_id(p_member_ptr)                              \
    cpphibernate_make_field_name(                                       \
        "p_id",                                                         \
        p_member_ptr,                                                   \
        cpphibernate::schema::attribute::primary_key)