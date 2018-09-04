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
                            "BinToUuid(`T0`.`tbl_test1_id`), "
                            "`T0`.`str_data`, "
                            "`T0`.`str64_data`, "
                            "`T0`.`u32_nullable`, "
                            "`T0`.`u32_ptr_u`, "
                            "`T0`.`u32_ptr_s` "
                        "FROM "
                            "`tbl_test1` AS `T0`   ",
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
                            "BinToUuid(`T0`.`tbl_test2_id`), "
                            "`T0`.`u8_data`, "
                            "`T0`.`i8_data`, "
                            "`T0`.`u16_data`, "
                            "`T0`.`i16_data` "
                        "FROM "
                            "`tbl_test2` AS `T0`   ",
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
                            "BinToUuid(`T0`.`tbl_test3_id`), "
                            "`T0`.`u32_data`, "
                            "`T0`.`i32_data`, "
                            "`T0`.`u64_data`, "
                            "`T0`.`i64_data` "
                        "FROM "
                            "`tbl_test3` AS `T0`   ",
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
                            "BinToUuid(`T1`.`tbl_base_id`), "
                            "`T1`.`name`, "
                            "BinToUuid(`T0`.`tbl_derived1_id`), "
                            "`T0`.`enum_data`, "
                            "BinToUuid(`T2`.`tbl_test1_id`), "
                            "`T2`.`str_data`, "
                            "`T2`.`str64_data`, "
                            "`T2`.`u32_nullable`, "
                            "`T2`.`u32_ptr_u`, "
                            "`T2`.`u32_ptr_s` "
                        "FROM "
                            "`tbl_derived1` AS `T0` "
                        "LEFT JOIN "
                            "`tbl_base` AS `T1` ON `T0`.`tbl_base_id`=`T1`.`tbl_base_id` "
                        "LEFT JOIN "
                            "`tbl_test1` AS `T2` ON `T0`.`tbl_test1_id_test1_data`=`T2`.`tbl_test1_id`   ",
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
                            "BinToUuid(`T1`.`tbl_base_id`), "
                            "`T1`.`name`, "
                            "BinToUuid(`T0`.`tbl_derived2_id`), "
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
                            "BinToUuid(`T4`.`tbl_test2_id`), "
                            "`T4`.`u8_data`, "
                            "`T4`.`i8_data`, "
                            "`T4`.`u16_data`, "
                            "`T4`.`i16_data` "
                        "FROM "
                            "`tbl_derived2` AS `T0` "
                        "LEFT JOIN "
                            "`tbl_base` AS `T1` ON `T0`.`tbl_base_id`=`T1`.`tbl_base_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T2` ON `T0`.`tbl_test2_id_test2_nullable`=`T2`.`tbl_test2_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T3` ON `T0`.`tbl_test2_id_test2_ptr_u`=`T3`.`tbl_test2_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T4` ON `T0`.`tbl_test2_id_test2_ptr_s`=`T4`.`tbl_test2_id`   ",
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
