#include <cpphibernate/driver/mariadb.h>

#include "test_helper.h"
#include "test_schema.h"
#include "mariadb_mock.h"

using namespace ::testing;
using namespace ::cpphibernate;
using namespace ::cpphibernate::modifier;
using namespace ::boost::hana::literals;

TEST(CppHibernateTests, read_test1)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "SELECT "
                            "BinToUuid(`tbl_test1`.`tbl_test1_id`), "
                            "`tbl_test1`.`str_data`, "
                            "`tbl_test1`.`str64_data`, "
                            "`tbl_test1`.`u32_nullable`, "
                            "`tbl_test1`.`u32_ptr_u`, "
                            "`tbl_test1`.`u32_ptr_s` "
                        "FROM "
                            "`tbl_test1` "
                        "WHERE "
                            "(`tbl_test1`.`tbl_test1_id`=UuidToBin('X3d12697a-abb9-11e8-98d0-529269fb1459X'))  ",
                        result_used({
                           { "3d12697a-abb9-11e8-98d0-529269fb1459", "str_data of class `test1` object `t1`", "str64_data of class `test1` object `t1`", nullptr, "123", "456" }
                        }));
    expect_query(mock,  "COMMIT");

    EXPECT_CALL(
        mock,
        mysql_close(
            reinterpret_cast<MYSQL*>(0x1111)));

    EXPECT_CALL(
        mock,
        mysql_real_escape_string(reinterpret_cast<MYSQL*>(0x1111), _, _, _))
            .Times(AnyNumber())
            .WillRepeatedly(WithArgs<1, 2, 3>(EscapeString()));

    test1 t1;
    t1.id = uuid("3d12697a-abb9-11e8-98d0-529269fb1459");

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);

    using namespace modifier;
    context.read(t1);

    EXPECT_EQ   (t1.id,         uuid("3d12697a-abb9-11e8-98d0-529269fb1459"));
    EXPECT_EQ   (t1.str_data,   "str_data of class `test1` object `t1`");
    EXPECT_EQ   (t1.str64_data, "str64_data of class `test1` object `t1`");
    EXPECT_FALSE(static_cast<bool>(t1.u32_nullable));
    ASSERT_TRUE (static_cast<bool>(t1.u32_ptr_u));
    EXPECT_EQ   (*t1.u32_ptr_u, 123);
    ASSERT_TRUE (static_cast<bool>(t1.u32_ptr_s));
    EXPECT_EQ   (*t1.u32_ptr_s, 456);
}

TEST(CppHibernateTests, read_test2)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "SELECT "
                            "BinToUuid(`tbl_test2`.`tbl_test2_id`), "
                            "`tbl_test2`.`u8_data`, "
                            "`tbl_test2`.`i8_data`, "
                            "`tbl_test2`.`u16_data`, "
                            "`tbl_test2`.`i16_data` "
                        "FROM "
                            "`tbl_test2` "
                        "WHERE "
                            "(`tbl_test2`.`tbl_test2_id`=UuidToBin('X3d1270dc-abb9-11e8-98d0-529269fb1459X'))  ",
                        result_used({
                            { "3d1270dc-abb9-11e8-98d0-529269fb1459", "1", "2", "3", "4" }
                        }));
    expect_query(mock,  "COMMIT");

    EXPECT_CALL(
        mock,
        mysql_real_escape_string(reinterpret_cast<MYSQL*>(0x1111), _, _, _))
            .Times(AnyNumber())
            .WillRepeatedly(WithArgs<1, 2, 3>(EscapeString()));

    EXPECT_CALL(
        mock,
        mysql_close(
            reinterpret_cast<MYSQL*>(0x1111)));

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);

    constexpr decltype(auto) test2_key_field = test_schema.tables[1_c].fields[0_c];

    test2 t2;
    context.read(t2, where(equal(test2_key_field, "3d1270dc-abb9-11e8-98d0-529269fb1459")));

    EXPECT_EQ(t2.id,       uuid("3d1270dc-abb9-11e8-98d0-529269fb1459"));
    EXPECT_EQ(t2.u8_data,  1);
    EXPECT_EQ(t2.i8_data,  2);
    EXPECT_EQ(t2.u16_data, 3);
    EXPECT_EQ(t2.i16_data, 4);
}

TEST(CppHibernateTests, read_test3)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "SELECT "
                            "BinToUuid(`tbl_test3`.`tbl_test3_id`), "
                            "`tbl_test3`.`u32_data`, "
                            "`tbl_test3`.`i32_data`, "
                            "`tbl_test3`.`u64_data`, "
                            "`tbl_test3`.`i64_data` "
                        "FROM "
                            "`tbl_test3` "
                        "WHERE "
                            "(`tbl_test3`.`tbl_test3_id`=UuidToBin('X3d12737a-abb9-11e8-98d0-529269fb1459X'))  ",
                        result_used({
                            { "3d12737a-abb9-11e8-98d0-529269fb1459", "5", "6", "7", "8" }
                        }));
    expect_query(mock,  "COMMIT");

    EXPECT_CALL(
        mock,
        mysql_real_escape_string(reinterpret_cast<MYSQL*>(0x1111), _, _, _))
            .Times(AnyNumber())
            .WillRepeatedly(WithArgs<1, 2, 3>(EscapeString()));

    EXPECT_CALL(
        mock,
        mysql_close(
            reinterpret_cast<MYSQL*>(0x1111)));

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);

    test3 t3;
    t3.id = uuid("3d12737a-abb9-11e8-98d0-529269fb1459");

    context.read(t3);

    EXPECT_EQ(t3.id,       uuid("3d12737a-abb9-11e8-98d0-529269fb1459"));
    EXPECT_EQ(t3.u32_data, 5);
    EXPECT_EQ(t3.i32_data, 6);
    EXPECT_EQ(t3.u64_data, 7);
    EXPECT_EQ(t3.i64_data, 8);
}

TEST(CppHibernateTests, read_derived1_static)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "SELECT "
                            "BinToUuid(`tbl_base`.`tbl_base_id`), "
                            "`tbl_base`.`name`, "
                            "BinToUuid(`tbl_derived1`.`tbl_derived1_id`), "
                            "`tbl_derived1`.`enum_data`, "
                            "BinToUuid(`T0`.`tbl_test1_id`), "
                            "`T0`.`str_data`, "
                            "`T0`.`str64_data`, "
                            "`T0`.`u32_nullable`, "
                            "`T0`.`u32_ptr_u`, "
                            "`T0`.`u32_ptr_s` "
                        "FROM "
                            "`tbl_derived1` "
                        "JOIN "
                            "`tbl_base` ON `tbl_derived1`.`tbl_base_id`=`tbl_base`.`tbl_base_id` "
                        "LEFT JOIN "
                            "`tbl_test1` AS `T0` ON `tbl_derived1`.`tbl_test1_id_test1_data`=`T0`.`tbl_test1_id` "
                        "WHERE "
                            "(`tbl_derived1`.`tbl_derived1_id`=UuidToBin('X3d12758c-abb9-11e8-98d0-529269fb1459X'))  ",
                        result_used({
                            { "3d12778a-abb9-11e8-98d0-529269fb1459", "derived1", "3d12758c-abb9-11e8-98d0-529269fb1459", "test2", "3d127988-abb9-11e8-98d0-529269fb1459", "str_data of class `test1` object `d1.test1_data`", "str64_data of class `test1` object `d1.test1_data`", "32", nullptr, "789" }
                        }));
    expect_query(mock,  "COMMIT");

    EXPECT_CALL(
        mock,
        mysql_real_escape_string(reinterpret_cast<MYSQL*>(0x1111), _, _, _))
            .Times(AnyNumber())
            .WillRepeatedly(WithArgs<1, 2, 3>(EscapeString()));

    EXPECT_CALL(
        mock,
        mysql_close(
            reinterpret_cast<MYSQL*>(0x1111)));

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);

    derived1 d1;
    d1.derived1_id = uuid("3d12758c-abb9-11e8-98d0-529269fb1459");

    context.read(d1);

    EXPECT_EQ   (d1.id,                        uuid("3d12778a-abb9-11e8-98d0-529269fb1459"));
    EXPECT_EQ   (d1.derived1_id,               uuid("3d12758c-abb9-11e8-98d0-529269fb1459"));
    EXPECT_EQ   (d1.name,                      "derived1");
    EXPECT_EQ   (d1.enum_data,                 test_enum::test2);
    EXPECT_EQ   (d1.test1_data.str_data,       "str_data of class `test1` object `d1.test1_data`");
    EXPECT_EQ   (d1.test1_data.str64_data,     "str64_data of class `test1` object `d1.test1_data`");
    ASSERT_TRUE (static_cast<bool>(d1.test1_data.u32_nullable));
    EXPECT_EQ   (*d1.test1_data.u32_nullable,   32);
    ASSERT_TRUE (static_cast<bool>(d1.test1_data.u32_ptr_s));
    EXPECT_EQ   (*d1.test1_data.u32_ptr_s,      789);
    EXPECT_FALSE(static_cast<bool>(d1.test1_data.u32_ptr_u));
}

TEST(CppHibernateTests, read_derived2_static)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "SELECT "
                            "BinToUuid(`tbl_base`.`tbl_base_id`), "
                            "`tbl_base`.`name`, "
                            "BinToUuid(`tbl_derived2`.`tbl_derived2_id`), "
                            "BinToUuid(`T0`.`tbl_test2_id`), "
                            "`T0`.`u8_data`, "
                            "`T0`.`i8_data`, "
                            "`T0`.`u16_data`, "
                            "`T0`.`i16_data`, "
                            "BinToUuid(`T1`.`tbl_test2_id`), "
                            "`T1`.`u8_data`, "
                            "`T1`.`i8_data`, "
                            "`T1`.`u16_data`, "
                            "`T1`.`i16_data`, "
                            "BinToUuid(`T2`.`tbl_test2_id`), "
                            "`T2`.`u8_data`, "
                            "`T2`.`i8_data`, "
                            "`T2`.`u16_data`, "
                            "`T2`.`i16_data` "
                        "FROM "
                            "`tbl_derived2` "
                        "JOIN "
                            "`tbl_base` ON `tbl_derived2`.`tbl_base_id`=`tbl_base`.`tbl_base_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T0` ON `tbl_derived2`.`tbl_test2_id_test2_nullable`=`T0`.`tbl_test2_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T1` ON `tbl_derived2`.`tbl_test2_id_test2_ptr_u`=`T1`.`tbl_test2_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T2` ON `tbl_derived2`.`tbl_test2_id_test2_ptr_s`=`T2`.`tbl_test2_id` "
                        "WHERE "
                            "(`tbl_derived2`.`tbl_derived2_id`=UuidToBin('X3d127bcc-abb9-11e8-98d0-529269fb1459X'))  ",
                        result_used({
                            { "3d127db6-abb9-11e8-98d0-529269fb1459", "derived2", "3d127bcc-abb9-11e8-98d0-529269fb1459", "3d1283a6-abb9-11e8-98d0-529269fb1459", "10", "11", "12", "13", "3d128522-abb9-11e8-98d0-529269fb1459", "20", "21", "22", "23", nullptr, nullptr, nullptr, nullptr, nullptr }
                        }));
    expect_query(mock,  "COMMIT");

    EXPECT_CALL(
        mock,
        mysql_real_escape_string(reinterpret_cast<MYSQL*>(0x1111), _, _, _))
            .Times(AnyNumber())
            .WillRepeatedly(WithArgs<1, 2, 3>(EscapeString()));

    EXPECT_CALL(
        mock,
        mysql_close(
            reinterpret_cast<MYSQL*>(0x1111)));

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);

    derived2 d2;
    d2.derived2_id = uuid("3d127bcc-abb9-11e8-98d0-529269fb1459");

    context.read(d2);

    EXPECT_EQ   (d2.id,                         uuid("3d127db6-abb9-11e8-98d0-529269fb1459"));
    EXPECT_EQ   (d2.derived2_id,                uuid("3d127bcc-abb9-11e8-98d0-529269fb1459"));
    EXPECT_EQ   (d2.name,                       "derived2");
    ASSERT_TRUE (static_cast<bool>(d2.test2_nullable));
    EXPECT_EQ   (d2.test2_nullable->u8_data,    10);
    EXPECT_EQ   (d2.test2_nullable->i8_data,    11);
    EXPECT_EQ   (d2.test2_nullable->u16_data,   12);
    EXPECT_EQ   (d2.test2_nullable->i16_data,   13);
    ASSERT_TRUE (static_cast<bool>(d2.test2_ptr_u));
    EXPECT_EQ   (d2.test2_ptr_u->u8_data,       20);
    EXPECT_EQ   (d2.test2_ptr_u->i8_data,       21);
    EXPECT_EQ   (d2.test2_ptr_u->u16_data,      22);
    EXPECT_EQ   (d2.test2_ptr_u->i16_data,      23);
    EXPECT_FALSE(static_cast<bool>(d2.test2_ptr_s));
}

TEST(CppHibernateTests, read_derived3_static)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "SELECT "
                            "BinToUuid(`tbl_base`.`tbl_base_id`), "
                            "`tbl_base`.`name`, "
                            "BinToUuid(`tbl_derived2`.`tbl_derived2_id`), "
                            "BinToUuid(`T0`.`tbl_test2_id`), "
                            "`T0`.`u8_data`, "
                            "`T0`.`i8_data`, "
                            "`T0`.`u16_data`, "
                            "`T0`.`i16_data`, "
                            "BinToUuid(`T1`.`tbl_test2_id`), "
                            "`T1`.`u8_data`, "
                            "`T1`.`i8_data`, "
                            "`T1`.`u16_data`, "
                            "`T1`.`i16_data`, "
                            "BinToUuid(`T2`.`tbl_test2_id`), "
                            "`T2`.`u8_data`, "
                            "`T2`.`i8_data`, "
                            "`T2`.`u16_data`, "
                            "`T2`.`i16_data`, "
                            "BinToUuid(`tbl_derived3`.`tbl_derived3_id`) "
                        "FROM "
                            "`tbl_derived3` "
                        "JOIN "
                            "`tbl_derived2` ON `tbl_derived3`.`tbl_derived2_id`=`tbl_derived2`.`tbl_derived2_id` "
                        "JOIN "
                            "`tbl_base` ON `tbl_derived2`.`tbl_base_id`=`tbl_base`.`tbl_base_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T0` ON `tbl_derived2`.`tbl_test2_id_test2_nullable`=`T0`.`tbl_test2_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T1` ON `tbl_derived2`.`tbl_test2_id_test2_ptr_u`=`T1`.`tbl_test2_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T2` ON `tbl_derived2`.`tbl_test2_id_test2_ptr_s`=`T2`.`tbl_test2_id` "
                        "WHERE "
                            "(`tbl_derived3`.`tbl_derived3_id`=UuidToBin('X3d12866c-abb9-11e8-98d0-529269fb1459X'))  ",
                        result_used({
                            { "3d1288ce-abb9-11e8-98d0-529269fb1459", "derived3", "3d1287a2-abb9-11e8-98d0-529269fb1459", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "3d12866c-abb9-11e8-98d0-529269fb1459" }
                        }));
    expect_query(mock,  "SELECT "
                            "BinToUuid(`tbl_test3`.`tbl_test3_id`), "
                            "`tbl_test3`.`u32_data`, "
                            "`tbl_test3`.`i32_data`, "
                            "`tbl_test3`.`u64_data`, "
                            "`tbl_test3`.`i64_data` "
                        "FROM "
                            "`tbl_test3` "
                        "WHERE "
                            "(`tbl_test3`.`tbl_derived3_id_test3_list`=UuidToBin('X3d12866c-abb9-11e8-98d0-529269fb1459X')) "
                        "ORDER BY "
                            "`tbl_test3`.`tbl_derived3_index_test3_list` ASC ",
                        result_used({
                            { "3d1289f0-abb9-11e8-98d0-529269fb1459", "100", "101", "102", "103" },
                            { "3d128b26-abb9-11e8-98d0-529269fb1459", "110", "111", "112", "113" },
                        }));
    expect_query(mock,  "SELECT "
                            "BinToUuid(`tbl_test3`.`tbl_test3_id`), "
                            "`tbl_test3`.`u32_data`, "
                            "`tbl_test3`.`i32_data`, "
                            "`tbl_test3`.`u64_data`, "
                            "`tbl_test3`.`i64_data` "
                        "FROM "
                            "`tbl_test3` "
                        "WHERE "
                            "(`tbl_test3`.`tbl_derived3_id_test3_vector`=UuidToBin('X3d12866c-abb9-11e8-98d0-529269fb1459X')) "
                        "ORDER BY "
                            "`tbl_test3`.`tbl_derived3_index_test3_vector` ASC ",
                        result_used({
                            { "3d128eb4-abb9-11e8-98d0-529269fb1459", "120", "121", "122", "123" },
                            { "3d128ffe-abb9-11e8-98d0-529269fb1459", "130", "131", "132", "133" },
                            { "3d129134-abb9-11e8-98d0-529269fb1459", "140", "141", "142", "143" },
                        }));
    expect_query(mock,  "COMMIT");

    EXPECT_CALL(
        mock,
        mysql_real_escape_string(reinterpret_cast<MYSQL*>(0x1111), _, _, _))
            .Times(AnyNumber())
            .WillRepeatedly(WithArgs<1, 2, 3>(EscapeString()));

    EXPECT_CALL(
        mock,
        mysql_close(
            reinterpret_cast<MYSQL*>(0x1111)));

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);

    derived3 d3;
    d3.derived3_id = uuid("3d12866c-abb9-11e8-98d0-529269fb1459");

    context.read(d3);

    EXPECT_EQ   (d3.id,            uuid("3d1288ce-abb9-11e8-98d0-529269fb1459"));
    EXPECT_EQ   (d3.name,          "derived3");
    EXPECT_EQ   (d3.derived2_id,   uuid("3d1287a2-abb9-11e8-98d0-529269fb1459"));
    EXPECT_FALSE(static_cast<bool>(d3.test2_nullable));
    EXPECT_FALSE(static_cast<bool>(d3.test2_ptr_u));
    EXPECT_FALSE(static_cast<bool>(d3.test2_ptr_s));
    EXPECT_EQ   (d3.derived3_id,   uuid("3d12866c-abb9-11e8-98d0-529269fb1459"));

    {
        auto it = d3.test3_list.begin();
        ASSERT_NE(it, d3.test3_list.end());
        EXPECT_EQ(it->id,       uuid("3d1289f0-abb9-11e8-98d0-529269fb1459"));
        EXPECT_EQ(it->u32_data, 100);
        EXPECT_EQ(it->i32_data, 101);
        EXPECT_EQ(it->u64_data, 102);
        EXPECT_EQ(it->i64_data, 103);
        ++it;

        ASSERT_NE(it, d3.test3_list.end());
        EXPECT_EQ(it->id,       uuid("3d128b26-abb9-11e8-98d0-529269fb1459"));
        EXPECT_EQ(it->u32_data, 110);
        EXPECT_EQ(it->i32_data, 111);
        EXPECT_EQ(it->u64_data, 112);
        EXPECT_EQ(it->i64_data, 113);
        ++it;

        EXPECT_EQ(it, d3.test3_list.end());
    }

    {
        auto it = d3.test3_vector.begin();
        ASSERT_NE(it, d3.test3_vector.end());
        EXPECT_EQ(it->id,       uuid("3d128eb4-abb9-11e8-98d0-529269fb1459"));
        EXPECT_EQ(it->u32_data, 120);
        EXPECT_EQ(it->i32_data, 121);
        EXPECT_EQ(it->u64_data, 122);
        EXPECT_EQ(it->i64_data, 123);
        ++it;

        ASSERT_NE(it, d3.test3_vector.end());
        EXPECT_EQ(it->id,       uuid("3d128ffe-abb9-11e8-98d0-529269fb1459"));
        EXPECT_EQ(it->u32_data, 130);
        EXPECT_EQ(it->i32_data, 131);
        EXPECT_EQ(it->u64_data, 132);
        EXPECT_EQ(it->i64_data, 133);
        ++it;

        ASSERT_NE(it, d3.test3_vector.end());
        EXPECT_EQ(it->id,       uuid("3d129134-abb9-11e8-98d0-529269fb1459"));
        EXPECT_EQ(it->u32_data, 140);
        EXPECT_EQ(it->i32_data, 141);
        EXPECT_EQ(it->u64_data, 142);
        EXPECT_EQ(it->i64_data, 143);
        ++it;

        EXPECT_EQ(it, d3.test3_vector.end());
    }
}

TEST(CppHibernateTests, read_derived2_ptr_dynamic)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "SELECT "
                            "`tbl_base`.`__type` AS `__type`, "
                            "BinToUuid(`tbl_base`.`tbl_base_id`), "
                            "`tbl_base`.`name`, "
                            "BinToUuid(`tbl_derived2`.`tbl_derived2_id`), "
                            "BinToUuid(`T0`.`tbl_test2_id`), "
                            "`T0`.`u8_data`, "
                            "`T0`.`i8_data`, "
                            "`T0`.`u16_data`, "
                            "`T0`.`i16_data`, "
                            "BinToUuid(`T1`.`tbl_test2_id`), "
                            "`T1`.`u8_data`, "
                            "`T1`.`i8_data`, "
                            "`T1`.`u16_data`, "
                            "`T1`.`i16_data`, "
                            "BinToUuid(`T2`.`tbl_test2_id`), "
                            "`T2`.`u8_data`, "
                            "`T2`.`i8_data`, "
                            "`T2`.`u16_data`, "
                            "`T2`.`i16_data`, "
                            "BinToUuid(`tbl_derived3`.`tbl_derived3_id`) "
                        "FROM "
                            "`tbl_derived2` "
                        "JOIN "
                            "`tbl_base` ON `tbl_derived2`.`tbl_base_id`=`tbl_base`.`tbl_base_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T0` ON `tbl_derived2`.`tbl_test2_id_test2_nullable`=`T0`.`tbl_test2_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T1` ON `tbl_derived2`.`tbl_test2_id_test2_ptr_u`=`T1`.`tbl_test2_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T2` ON `tbl_derived2`.`tbl_test2_id_test2_ptr_s`=`T2`.`tbl_test2_id` "
                        "LEFT JOIN "
                            "`tbl_derived3` ON `tbl_derived2`.`tbl_derived2_id`=`tbl_derived3`.`tbl_derived2_id` "
                        "WHERE "
                            "(`tbl_derived2`.`tbl_derived2_id`=UuidToBin('X3d1287a2-abb9-11e8-98d0-529269fb1459X'))  ",
                        result_used({
                            {
                                /* base */                      "13", "3d1288ce-abb9-11e8-98d0-529269fb1459", "derived3",
                                /* derived2 */                  "3d1287a2-abb9-11e8-98d0-529269fb1459",
                                /* derived2.test2_nullable */   nullptr, nullptr, nullptr, nullptr, nullptr,
                                /* derived2.test2_ptr_u */      nullptr, nullptr, nullptr, nullptr, nullptr,
                                /* derived2.test2_ptr_s */      nullptr, nullptr, nullptr, nullptr, nullptr,
                                /* derived3 */                  "3d12866c-abb9-11e8-98d0-529269fb1459"
                            }
                        }));
    expect_query(mock,  "SELECT "
                            "BinToUuid(`tbl_test3`.`tbl_test3_id`), "
                            "`tbl_test3`.`u32_data`, "
                            "`tbl_test3`.`i32_data`, "
                            "`tbl_test3`.`u64_data`, "
                            "`tbl_test3`.`i64_data` "
                        "FROM "
                            "`tbl_test3` "
                        "WHERE "
                            "(`tbl_test3`.`tbl_derived3_id_test3_list`=UuidToBin('X3d12866c-abb9-11e8-98d0-529269fb1459X')) "
                        "ORDER BY "
                            "`tbl_test3`.`tbl_derived3_index_test3_list` ASC ",
                        result_used({
                            { "3d1289f0-abb9-11e8-98d0-529269fb1459", "100", "101", "102", "103" },
                            { "3d128b26-abb9-11e8-98d0-529269fb1459", "110", "111", "112", "113" },
                        }));
    expect_query(mock,  "SELECT "
                            "BinToUuid(`tbl_test3`.`tbl_test3_id`), "
                            "`tbl_test3`.`u32_data`, "
                            "`tbl_test3`.`i32_data`, "
                            "`tbl_test3`.`u64_data`, "
                            "`tbl_test3`.`i64_data` "
                        "FROM "
                            "`tbl_test3` "
                        "WHERE "
                            "(`tbl_test3`.`tbl_derived3_id_test3_vector`=UuidToBin('X3d12866c-abb9-11e8-98d0-529269fb1459X')) "
                        "ORDER BY "
                            "`tbl_test3`.`tbl_derived3_index_test3_vector` ASC ",
                        result_used({
                            { "3d128eb4-abb9-11e8-98d0-529269fb1459", "120", "121", "122", "123" },
                            { "3d128ffe-abb9-11e8-98d0-529269fb1459", "130", "131", "132", "133" },
                            { "3d129134-abb9-11e8-98d0-529269fb1459", "140", "141", "142", "143" },
                        }));
    expect_query(mock,  "COMMIT");

    EXPECT_CALL(
        mock,
        mysql_real_escape_string(reinterpret_cast<MYSQL*>(0x1111), _, _, _))
            .Times(AnyNumber())
            .WillRepeatedly(WithArgs<1, 2, 3>(EscapeString()));

    EXPECT_CALL(
        mock,
        mysql_close(
            reinterpret_cast<MYSQL*>(0x1111)));

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);

    constexpr decltype(auto) d2_key_field = test_schema.tables[5_c].fields[0_c];

    std::unique_ptr<derived2> d2_ptr;
    context.read(d2_ptr, where(equal(d2_key_field, "3d1287a2-abb9-11e8-98d0-529269fb1459")));
    auto* d3_ptr = dynamic_cast<derived3*>(d2_ptr.get());
    ASSERT_TRUE (d3_ptr);
    auto& d3 = *d3_ptr;
    EXPECT_EQ   (d3.id,            uuid("3d1288ce-abb9-11e8-98d0-529269fb1459"));
    EXPECT_EQ   (d3.name,          "derived3");
    EXPECT_EQ   (d3.derived2_id,   uuid("3d1287a2-abb9-11e8-98d0-529269fb1459"));
    EXPECT_FALSE(static_cast<bool>(d3.test2_nullable));
    EXPECT_FALSE(static_cast<bool>(d3.test2_ptr_u));
    EXPECT_FALSE(static_cast<bool>(d3.test2_ptr_s));
    EXPECT_EQ   (d3.derived3_id,   uuid("3d12866c-abb9-11e8-98d0-529269fb1459"));

    {
        auto it = d3.test3_list.begin();
        ASSERT_NE(it, d3.test3_list.end());
        EXPECT_EQ(it->id,       uuid("3d1289f0-abb9-11e8-98d0-529269fb1459"));
        EXPECT_EQ(it->u32_data, 100);
        EXPECT_EQ(it->i32_data, 101);
        EXPECT_EQ(it->u64_data, 102);
        EXPECT_EQ(it->i64_data, 103);
        ++it;

        ASSERT_NE(it, d3.test3_list.end());
        EXPECT_EQ(it->id,       uuid("3d128b26-abb9-11e8-98d0-529269fb1459"));
        EXPECT_EQ(it->u32_data, 110);
        EXPECT_EQ(it->i32_data, 111);
        EXPECT_EQ(it->u64_data, 112);
        EXPECT_EQ(it->i64_data, 113);
        ++it;

        EXPECT_EQ(it, d3.test3_list.end());
    }

    {
        auto it = d3.test3_vector.begin();
        ASSERT_NE(it, d3.test3_vector.end());
        EXPECT_EQ(it->id,       uuid("3d128eb4-abb9-11e8-98d0-529269fb1459"));
        EXPECT_EQ(it->u32_data, 120);
        EXPECT_EQ(it->i32_data, 121);
        EXPECT_EQ(it->u64_data, 122);
        EXPECT_EQ(it->i64_data, 123);
        ++it;

        ASSERT_NE(it, d3.test3_vector.end());
        EXPECT_EQ(it->id,       uuid("3d128ffe-abb9-11e8-98d0-529269fb1459"));
        EXPECT_EQ(it->u32_data, 130);
        EXPECT_EQ(it->i32_data, 131);
        EXPECT_EQ(it->u64_data, 132);
        EXPECT_EQ(it->i64_data, 133);
        ++it;

        ASSERT_NE(it, d3.test3_vector.end());
        EXPECT_EQ(it->id,       uuid("3d129134-abb9-11e8-98d0-529269fb1459"));
        EXPECT_EQ(it->u32_data, 140);
        EXPECT_EQ(it->i32_data, 141);
        EXPECT_EQ(it->u64_data, 142);
        EXPECT_EQ(it->i64_data, 143);
        ++it;

        EXPECT_EQ(it, d3.test3_vector.end());
    }
}

TEST(CppHibernateTests, read_base_ptr_vector_dynamic)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "SELECT "
                            "`tbl_base`.`__type` AS `__type`, "
                            "BinToUuid(`tbl_base`.`tbl_base_id`), "
                            "`tbl_base`.`name`, "
                            "BinToUuid(`tbl_derived1`.`tbl_derived1_id`), "
                            "`tbl_derived1`.`enum_data`, "
                            "BinToUuid(`T0`.`tbl_test1_id`), "
                            "`T0`.`str_data`, "
                            "`T0`.`str64_data`, "
                            "`T0`.`u32_nullable`, "
                            "`T0`.`u32_ptr_u`, "
                            "`T0`.`u32_ptr_s`, "
                            "BinToUuid(`tbl_derived2`.`tbl_derived2_id`), "
                            "BinToUuid(`T1`.`tbl_test2_id`), "
                            "`T1`.`u8_data`, "
                            "`T1`.`i8_data`, "
                            "`T1`.`u16_data`, "
                            "`T1`.`i16_data`, "
                            "BinToUuid(`T2`.`tbl_test2_id`), "
                            "`T2`.`u8_data`, "
                            "`T2`.`i8_data`, "
                            "`T2`.`u16_data`, "
                            "`T2`.`i16_data`, "
                            "BinToUuid(`T3`.`tbl_test2_id`), "
                            "`T3`.`u8_data`, "
                            "`T3`.`i8_data`, "
                            "`T3`.`u16_data`, "
                            "`T3`.`i16_data`, "
                            "BinToUuid(`tbl_derived3`.`tbl_derived3_id`) "
                        "FROM "
                            "`tbl_base` "
                        "LEFT JOIN "
                            "`tbl_derived1` ON `tbl_base`.`tbl_base_id`=`tbl_derived1`.`tbl_base_id` "
                        "LEFT JOIN "
                            "`tbl_test1` AS `T0` ON `tbl_derived1`.`tbl_test1_id_test1_data`=`T0`.`tbl_test1_id` "
                        "LEFT JOIN "
                            "`tbl_derived2` ON `tbl_base`.`tbl_base_id`=`tbl_derived2`.`tbl_base_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T1` ON `tbl_derived2`.`tbl_test2_id_test2_nullable`=`T1`.`tbl_test2_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T2` ON `tbl_derived2`.`tbl_test2_id_test2_ptr_u`=`T2`.`tbl_test2_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T3` ON `tbl_derived2`.`tbl_test2_id_test2_ptr_s`=`T3`.`tbl_test2_id` "
                        "LEFT JOIN "
                            "`tbl_derived3` ON `tbl_derived2`.`tbl_derived2_id`=`tbl_derived3`.`tbl_derived2_id`   ",
                        result_used({
                            {
                                /* base */                      "11", "3d12778a-abb9-11e8-98d0-529269fb1459", "derived1",
                                /* derived1 */                  "3d12758c-abb9-11e8-98d0-529269fb1459", "test2",
                                /* derived1.test1 */            "3d127988-abb9-11e8-98d0-529269fb1459", "str_data of class `test1` object `d1.test1_data`", "str64_data of class `test1` object `d1.test1_data`", "32", nullptr, "789",
                                /* derived2 */                  nullptr,
                                /* derived2.test2_nullable */   nullptr, nullptr, nullptr, nullptr, nullptr,
                                /* derived2.test2_ptr_u */      nullptr, nullptr, nullptr, nullptr, nullptr,
                                /* derived2.test2_ptr_s */      nullptr, nullptr, nullptr, nullptr, nullptr,
                                /* derived3 */                  nullptr
                            },
                            {
                                /* base */                      "12", "3d127db6-abb9-11e8-98d0-529269fb1459", "derived2",
                                /* derived1 */                  nullptr, nullptr,
                                /* derived1.test1 */            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                                /* derived2 */                  "3d127bcc-abb9-11e8-98d0-529269fb1459",
                                /* derived2.test2_nullable */   "3d1283a6-abb9-11e8-98d0-529269fb1459", "10", "11", "12", "13",
                                /* derived2.test2_ptr_u */      "3d128522-abb9-11e8-98d0-529269fb1459", "20", "21", "22", "23",
                                /* derived2.test2_ptr_s */      nullptr, nullptr, nullptr, nullptr, nullptr,
                                /* derived3 */                  nullptr
                            },
                            {
                                /* base */                      "13", "3d1288ce-abb9-11e8-98d0-529269fb1459", "derived3",
                                /* derived1 */                  nullptr, nullptr,
                                /* derived1.test1 */            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                                /* derived2 */                  "3d1287a2-abb9-11e8-98d0-529269fb1459",
                                /* derived2.test2_nullable */   nullptr, nullptr, nullptr, nullptr, nullptr,
                                /* derived2.test2_ptr_u */      nullptr, nullptr, nullptr, nullptr, nullptr,
                                /* derived2.test2_ptr_s */      nullptr, nullptr, nullptr, nullptr, nullptr,
                                /* derived3 */                  "3d12866c-abb9-11e8-98d0-529269fb1459"
                            },
                        }));
    expect_query(mock,  "SELECT "
                            "BinToUuid(`tbl_test3`.`tbl_test3_id`), "
                            "`tbl_test3`.`u32_data`, "
                            "`tbl_test3`.`i32_data`, "
                            "`tbl_test3`.`u64_data`, "
                            "`tbl_test3`.`i64_data` "
                        "FROM "
                            "`tbl_test3` "
                        "WHERE "
                            "(`tbl_test3`.`tbl_derived3_id_test3_list`=UuidToBin('X3d12866c-abb9-11e8-98d0-529269fb1459X')) "
                        "ORDER BY "
                            "`tbl_test3`.`tbl_derived3_index_test3_list` ASC ",
                        result_used({
                            { "3d1289f0-abb9-11e8-98d0-529269fb1459", "100", "101", "102", "103" },
                            { "3d128b26-abb9-11e8-98d0-529269fb1459", "110", "111", "112", "113" },
                        }));
    expect_query(mock,  "SELECT "
                            "BinToUuid(`tbl_test3`.`tbl_test3_id`), "
                            "`tbl_test3`.`u32_data`, "
                            "`tbl_test3`.`i32_data`, "
                            "`tbl_test3`.`u64_data`, "
                            "`tbl_test3`.`i64_data` "
                        "FROM "
                            "`tbl_test3` "
                        "WHERE "
                            "(`tbl_test3`.`tbl_derived3_id_test3_vector`=UuidToBin('X3d12866c-abb9-11e8-98d0-529269fb1459X')) "
                        "ORDER BY "
                            "`tbl_test3`.`tbl_derived3_index_test3_vector` ASC ",
                        result_used({
                            { "3d128eb4-abb9-11e8-98d0-529269fb1459", "120", "121", "122", "123" },
                            { "3d128ffe-abb9-11e8-98d0-529269fb1459", "130", "131", "132", "133" },
                            { "3d129134-abb9-11e8-98d0-529269fb1459", "140", "141", "142", "143" },
                        }));
    expect_query(mock,  "COMMIT");

    EXPECT_CALL(
        mock,
        mysql_real_escape_string(reinterpret_cast<MYSQL*>(0x1111), _, _, _))
            .Times(AnyNumber())
            .WillRepeatedly(WithArgs<1, 2, 3>(EscapeString()));

    EXPECT_CALL(
        mock,
        mysql_close(
            reinterpret_cast<MYSQL*>(0x1111)));

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);

    using base_ptr_type = std::unique_ptr<base>;
    using base_vec_type = std::vector<base_ptr_type>;

    base_vec_type base_vec;
    context.read(base_vec);

    auto bIt = base_vec.begin();
    {
        ASSERT_NE  (bIt, base_vec.end());
        auto* base_ptr = bIt->get();
        ASSERT_TRUE(static_cast<bool>(base_ptr));
        auto* d1_ptr = dynamic_cast<derived1*>(base_ptr);
        ASSERT_TRUE(static_cast<bool>(d1_ptr));
        auto& d1 = *d1_ptr;
        ++bIt;

        EXPECT_EQ   (d1.id,                        uuid("3d12778a-abb9-11e8-98d0-529269fb1459"));
        EXPECT_EQ   (d1.derived1_id,               uuid("3d12758c-abb9-11e8-98d0-529269fb1459"));
        EXPECT_EQ   (d1.name,                      "derived1");
        EXPECT_EQ   (d1.enum_data,                 test_enum::test2);
        EXPECT_EQ   (d1.test1_data.str_data,       "str_data of class `test1` object `d1.test1_data`");
        EXPECT_EQ   (d1.test1_data.str64_data,     "str64_data of class `test1` object `d1.test1_data`");
        ASSERT_TRUE (static_cast<bool>(d1.test1_data.u32_nullable));
        EXPECT_EQ   (*d1.test1_data.u32_nullable,   32);
        ASSERT_TRUE (static_cast<bool>(d1.test1_data.u32_ptr_s));
        EXPECT_EQ   (*d1.test1_data.u32_ptr_s,      789);
        EXPECT_FALSE(static_cast<bool>(d1.test1_data.u32_ptr_u));
    }

    {
        ASSERT_NE  (bIt, base_vec.end());
        auto* base_ptr = bIt->get();
        ASSERT_TRUE(static_cast<bool>(base_ptr));
        auto* d2_ptr = dynamic_cast<derived2*>(base_ptr);
        ASSERT_TRUE(static_cast<bool>(d2_ptr));
        auto& d2 = *d2_ptr;
        ++bIt;

        EXPECT_EQ   (d2.id,                         uuid("3d127db6-abb9-11e8-98d0-529269fb1459"));
        EXPECT_EQ   (d2.derived2_id,                uuid("3d127bcc-abb9-11e8-98d0-529269fb1459"));
        EXPECT_EQ   (d2.name,                       "derived2");
        ASSERT_TRUE (static_cast<bool>(d2.test2_nullable));
        EXPECT_EQ   (d2.test2_nullable->u8_data,    10);
        EXPECT_EQ   (d2.test2_nullable->i8_data,    11);
        EXPECT_EQ   (d2.test2_nullable->u16_data,   12);
        EXPECT_EQ   (d2.test2_nullable->i16_data,   13);
        ASSERT_TRUE (static_cast<bool>(d2.test2_ptr_u));
        EXPECT_EQ   (d2.test2_ptr_u->u8_data,       20);
        EXPECT_EQ   (d2.test2_ptr_u->i8_data,       21);
        EXPECT_EQ   (d2.test2_ptr_u->u16_data,      22);
        EXPECT_EQ   (d2.test2_ptr_u->i16_data,      23);
        EXPECT_FALSE(static_cast<bool>(d2.test2_ptr_s));
    }

    {
        ASSERT_NE  (bIt, base_vec.end());
        auto* base_ptr = bIt->get();
        ASSERT_TRUE(static_cast<bool>(base_ptr));
        auto* d3_ptr = dynamic_cast<derived3*>(base_ptr);
        ASSERT_TRUE(static_cast<bool>(d3_ptr));
        auto& d3 = *d3_ptr;
        ++bIt;

        EXPECT_EQ   (d3.id,            uuid("3d1288ce-abb9-11e8-98d0-529269fb1459"));
        EXPECT_EQ   (d3.name,          "derived3");
        EXPECT_EQ   (d3.derived2_id,   uuid("3d1287a2-abb9-11e8-98d0-529269fb1459"));
        EXPECT_FALSE(static_cast<bool>(d3.test2_nullable));
        EXPECT_FALSE(static_cast<bool>(d3.test2_ptr_u));
        EXPECT_FALSE(static_cast<bool>(d3.test2_ptr_s));
        EXPECT_EQ   (d3.derived3_id,   uuid("3d12866c-abb9-11e8-98d0-529269fb1459"));

        {
            auto it = d3.test3_list.begin();
            ASSERT_NE(it, d3.test3_list.end());
            EXPECT_EQ(it->id,       uuid("3d1289f0-abb9-11e8-98d0-529269fb1459"));
            EXPECT_EQ(it->u32_data, 100);
            EXPECT_EQ(it->i32_data, 101);
            EXPECT_EQ(it->u64_data, 102);
            EXPECT_EQ(it->i64_data, 103);
            ++it;

            ASSERT_NE(it, d3.test3_list.end());
            EXPECT_EQ(it->id,       uuid("3d128b26-abb9-11e8-98d0-529269fb1459"));
            EXPECT_EQ(it->u32_data, 110);
            EXPECT_EQ(it->i32_data, 111);
            EXPECT_EQ(it->u64_data, 112);
            EXPECT_EQ(it->i64_data, 113);
            ++it;

            EXPECT_EQ(it, d3.test3_list.end());
        }

        {
            auto it = d3.test3_vector.begin();
            ASSERT_NE(it, d3.test3_vector.end());
            EXPECT_EQ(it->id,       uuid("3d128eb4-abb9-11e8-98d0-529269fb1459"));
            EXPECT_EQ(it->u32_data, 120);
            EXPECT_EQ(it->i32_data, 121);
            EXPECT_EQ(it->u64_data, 122);
            EXPECT_EQ(it->i64_data, 123);
            ++it;

            ASSERT_NE(it, d3.test3_vector.end());
            EXPECT_EQ(it->id,       uuid("3d128ffe-abb9-11e8-98d0-529269fb1459"));
            EXPECT_EQ(it->u32_data, 130);
            EXPECT_EQ(it->i32_data, 131);
            EXPECT_EQ(it->u64_data, 132);
            EXPECT_EQ(it->i64_data, 133);
            ++it;

            ASSERT_NE(it, d3.test3_vector.end());
            EXPECT_EQ(it->id,       uuid("3d129134-abb9-11e8-98d0-529269fb1459"));
            EXPECT_EQ(it->u32_data, 140);
            EXPECT_EQ(it->i32_data, 141);
            EXPECT_EQ(it->u64_data, 142);
            EXPECT_EQ(it->i64_data, 143);
            ++it;

            EXPECT_EQ(it, d3.test3_vector.end());
        }
    }

    EXPECT_EQ(bIt, base_vec.end());
}