#pragma once

#include <cpphibernate.h>
#include <cpputils/misc/enum.h>

enum class test_enum
{
    test0,
    test1,
    test2,
    test3,

    first = test0,
    last  = test3,
};

DEFINE_ENUM_TO_STRING_MAP(
    test_enum,
    { test_enum::test0, "test0" },
    { test_enum::test1, "test1" },
    { test_enum::test2, "test2" },
    { test_enum::test3, "test3" }
);

DEFINE_STRING_TO_ENUM_MAP(
    test_enum,
    invariant_string_less,
    { "test0", test_enum::test0 },
    { "test1", test_enum::test1 },
    { "test2", test_enum::test2 },
    { "test3", test_enum::test3 }
);

struct test1
{
    ::cpphibernate::uuid        id;
    std::string                 str_data;
    ::cpphibernate::string<64>  str64_data;

    utl::nullable<uint32_t>     u32_nullable;
    std::unique_ptr<uint32_t>   u32_ptr_u;
    std::shared_ptr<uint32_t>   u32_ptr_s;
};

struct test2
{
    ::cpphibernate::uuid        id;
    uint8_t                     u8_data;
    int8_t                      i8_data;
    uint16_t                    u16_data;
    int16_t                     i16_data;
};

struct test3
{
    ::cpphibernate::uuid        id;
    uint32_t                    u32_data;
    int32_t                     i32_data;
    uint64_t                    u64_data;
    int64_t                     i64_data;
};

struct base
{
    ::cpphibernate::uuid        id;
    std::string                 name;

    virtual ~base() = default;
};

struct derived1
    : public base
{
    ::cpphibernate::uuid        derived1_id;
    test1                       test1_data;
    test_enum                   enum_data;
};

struct derived2
    : public base
{
    ::cpphibernate::uuid        derived2_id;
    utl::nullable<test2>        test2_nullable;
    std::unique_ptr<test2>      test2_ptr_u;
    std::shared_ptr<test2>      test2_ptr_s;
};

struct derived3
    : public derived2
{
    ::cpphibernate::uuid        derived3_id;
    std::list<test3>            test3_list;
    std::vector<test3>          test3_vector;
};

constexpr decltype(auto) test_schema = cpphibernate_make_schema(
    test,
    cpphibernate_make_table_name(
        tbl_test1,
        test1,
        1,
        cpphibernate_make_id          (&test1::id),
        cpphibernate_make_field       (test1, str_data),
        cpphibernate_make_field       (test1, str64_data),
        cpphibernate_make_field       (test1, u32_nullable),
        cpphibernate_make_field       (test1, u32_ptr_u),
        cpphibernate_make_field       (test1, u32_ptr_s)
    ),
    cpphibernate_make_table_name(
        tbl_test2,
        test2,
        2,
        cpphibernate_make_id          (&test2::id),
        cpphibernate_make_field       (test2, u8_data),
        cpphibernate_make_field       (test2, i8_data),
        cpphibernate_make_field       (test2, u16_data),
        cpphibernate_make_field       (test2, i16_data)
    ),
    cpphibernate_make_table_name(
        tbl_test3,
        test3,
        3,
        cpphibernate_make_id          (&test3::id),
        cpphibernate_make_field       (test3, u32_data),
        cpphibernate_make_field       (test3, i32_data),
        cpphibernate_make_field       (test3, u64_data),
        cpphibernate_make_field       (test3, i64_data)
    ),

    cpphibernate_make_table_name(
        tbl_base,
        base,
        10,
        cpphibernate_make_id          (&base::id),
        cpphibernate_make_field       (base, name)
    ),
    cpphibernate_make_table_name(
        tbl_derived1,
        derived1,
        11,
        cpphibernate_make_id          (&derived1::derived1_id),
        cpphibernate_make_field       (derived1, test1_data),
        cpphibernate_make_field       (derived1, enum_data)
    ),
    cpphibernate_make_table_name(
        tbl_derived2,
        derived2,
        12,
        cpphibernate_make_id          (&derived2::derived2_id),
        cpphibernate_make_field       (derived2, test2_nullable),
        cpphibernate_make_field       (derived2, test2_ptr_u),
        cpphibernate_make_field       (derived2, test2_ptr_s)
    ),
    cpphibernate_make_table_name(
        tbl_derived3,
        derived3,
        13,
        cpphibernate_make_id          (&derived3::derived3_id),
        cpphibernate_make_field       (derived3, test3_list),
        cpphibernate_make_field       (derived3, test3_vector)
    )
);