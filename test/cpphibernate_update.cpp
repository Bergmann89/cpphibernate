#include <cpphibernate/driver/mariadb.h>

#include "test_helper.h"
#include "test_schema.h"
#include "mariadb_mock.h"

using namespace ::testing;
using namespace ::cpphibernate;

TEST(CppHibernateTests, update_test1)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "UPDATE "
                            "`tbl_test1` "
                        "SET "
                            "`str_data`='Xstr_data of class `test1` object `t1`X', "
                            "`str64_data`='Xstr64_data of class `test1` object `t1`X', "
                            "`u32_nullable`=null, "
                            "`u32_ptr_u`='X456X', "
                            "`u32_ptr_s`='X789X' "
                        "WHERE "
                            "`tbl_test1_id`=UuidToBin('X3d12697a-abb9-11e8-98d0-529269fb1459X')",
                        result_affected_rows(1));
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

    test1 t1;
    t1.id           = uuid("3d12697a-abb9-11e8-98d0-529269fb1459");
    t1.str_data     = "str_data of class `test1` object `t1`";
    t1.str64_data   = "str64_data of class `test1` object `t1`";
    t1.u32_ptr_u    = std::make_unique<uint32_t>(456);
    t1.u32_ptr_s    = std::make_shared<uint32_t>(789);

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);
    context.update(t1);
}

TEST(CppHibernateTests, update_test2)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "UPDATE "
                            "`tbl_test2` "
                        "SET "
                            "`u8_data`='X1X', "
                            "`i8_data`='X2X', "
                            "`u16_data`='X3X', "
                            "`i16_data`='X4X' "
                        "WHERE "
                            "`tbl_test2_id`=UuidToBin('X3d1270dc-abb9-11e8-98d0-529269fb1459X')",
                        result_affected_rows(1));
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

    test2 t2;
    t2.id       = uuid("3d1270dc-abb9-11e8-98d0-529269fb1459");
    t2.u8_data  = 1;
    t2.i8_data  = 2;
    t2.u16_data = 3;
    t2.i16_data = 4;

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);
    context.update(t2);
}

TEST(CppHibernateTests, update_test3)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "UPDATE "
                            "`tbl_test3` "
                        "SET "
                            "`u32_data`='X5X', "
                            "`i32_data`='X6X', "
                            "`u64_data`='X7X', "
                            "`i64_data`='X8X' "
                        "WHERE "
                            "`tbl_test3_id`=UuidToBin('X3d12737a-abb9-11e8-98d0-529269fb1459X')",
                        result_affected_rows(1));
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

    test3 t3;
    t3.id       = uuid("3d12737a-abb9-11e8-98d0-529269fb1459");
    t3.u32_data = 5;
    t3.i32_data = 6;
    t3.u64_data = 7;
    t3.i64_data = 8;

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);
    context.update(t3);
}

TEST(CppHibernateTests, update_derived1)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "UPDATE "
                            "`tbl_base` "
                        "SET "
                            "`name`='Xderived1X' "
                        "WHERE "
                            "`tbl_base_id`=UuidToBin('X3d12778a-abb9-11e8-98d0-529269fb1459X')",
                        result_affected_rows(1));
    expect_query(mock,  "SELECT Uuid()", result_used({
                            { "e2488a64-b843-11e8-96f8-529269fb1459" }
                        }));
    expect_query(mock,  "INSERT INTO "
                            "`tbl_test1` "
                        "SET "
                            "`tbl_test1_id`=UuidToBin('Xe2488a64-b843-11e8-96f8-529269fb1459X'), "
                            "`str_data`='Xstr_data of class `test1` object `d1.test1_data`X', "
                            "`str64_data`='Xstr64_data of class `test1` object `d1.test1_data`X', "
                            "`u32_nullable`='X32X', "
                            "`u32_ptr_u`=null, "
                            "`u32_ptr_s`='X789X'",
                        result_affected_rows(1));
    expect_query(mock,  "DELETE FROM "
                            "`tbl_test1` "
                        "WHERE "
                            "`tbl_test1_id` IN ("
                                "SELECT "
                                    "`tbl_test1_id_test1_data` "
                                "FROM "
                                    "`tbl_derived1` "
                                "WHERE "
                                    "`tbl_derived1_id`=UuidToBin('X3d12758c-abb9-11e8-98d0-529269fb1459X') AND "
                                    "`tbl_test1_id_test1_data`!= UuidToBin('Xe2488a64-b843-11e8-96f8-529269fb1459X')"
                            ")");
    expect_query(mock,  "UPDATE "
                            "`tbl_derived1` "
                        "SET "
                            "`tbl_base_id`=UuidToBin('X3d12778a-abb9-11e8-98d0-529269fb1459X'), "
                            "`tbl_test1_id_test1_data`=UuidToBin('Xe2488a64-b843-11e8-96f8-529269fb1459X'), "
                            "`enum_data`='Xtest2X' "
                        "WHERE "
                            "`tbl_derived1_id`=UuidToBin('X3d12758c-abb9-11e8-98d0-529269fb1459X')",
                        result_affected_rows(1));
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

    derived1 d1;
    d1.id                       = uuid("3d12778a-abb9-11e8-98d0-529269fb1459");
    d1.name                     = "derived1";
    d1.derived1_id              = uuid("3d12758c-abb9-11e8-98d0-529269fb1459");
    d1.enum_data                = test_enum::test2;
    d1.test1_data.str_data      = "str_data of class `test1` object `d1.test1_data`";
    d1.test1_data.str64_data    = "str64_data of class `test1` object `d1.test1_data`";
    d1.test1_data.u32_nullable  = 32;
    d1.test1_data.u32_ptr_s     = std::make_shared<uint32_t>(789);

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);
    context.update(static_cast<base&>(d1));
}

TEST(CppHibernateTests, update_derived2)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "UPDATE "
                            "`tbl_base` "
                        "SET "
                            "`name`='Xderived2X' "
                        "WHERE "
                            "`tbl_base_id`=UuidToBin('X3d127db6-abb9-11e8-98d0-529269fb1459X')",
                        result_affected_rows(1));
    expect_query(mock,  "UPDATE "
                            "`tbl_test2` "
                        "SET "
                            "`u8_data`='X10X', "
                            "`i8_data`='X11X', "
                            "`u16_data`='X12X', "
                            "`i16_data`='X13X' "
                        "WHERE "
                            "`tbl_test2_id`=UuidToBin('X3d1283a6-abb9-11e8-98d0-529269fb1459X')",
                        result_affected_rows(1));
    expect_query(mock,  "DELETE FROM "
                            "`tbl_test2` "
                        "WHERE "
                            "`tbl_test2_id` IN ("
                                "SELECT "
                                    "`tbl_test2_id_test2_nullable` "
                                "FROM "
                                    "`tbl_derived2` "
                                "WHERE "
                                    "`tbl_derived2_id`=UuidToBin('X3d127bcc-abb9-11e8-98d0-529269fb1459X') AND "
                                    "`tbl_test2_id_test2_nullable`!= UuidToBin('X3d1283a6-abb9-11e8-98d0-529269fb1459X')"
                            ")");
    expect_query(mock,  "SELECT Uuid()",
                        result_used({
                            { "ec0f0aac-b8b9-11e8-96f8-529269fb1459" }
                        }));
    expect_query(mock,  "INSERT INTO "
                            "`tbl_test2` "
                        "SET "
                            "`tbl_test2_id`=UuidToBin('Xec0f0aac-b8b9-11e8-96f8-529269fb1459X'), "
                            "`u8_data`='X20X', "
                            "`i8_data`='X21X', "
                            "`u16_data`='X22X', "
                            "`i16_data`='X23X'",
                        result_affected_rows(1));
    expect_query(mock,  "DELETE FROM "
                            "`tbl_test2` "
                        "WHERE "
                            "`tbl_test2_id` IN ("
                                "SELECT "
                                    "`tbl_test2_id_test2_ptr_u` "
                                "FROM "
                                    "`tbl_derived2` "
                                "WHERE "
                                    "`tbl_derived2_id`=UuidToBin('X3d127bcc-abb9-11e8-98d0-529269fb1459X') AND "
                                    "`tbl_test2_id_test2_ptr_u`!= UuidToBin('Xec0f0aac-b8b9-11e8-96f8-529269fb1459X')"
                            ")");
    expect_query(mock,  "DELETE FROM "
                            "`tbl_test2` "
                        "WHERE "
                            "`tbl_test2_id` IN ("
                                "SELECT "
                                    "`tbl_test2_id_test2_ptr_s` "
                                "FROM "
                                    "`tbl_derived2` "
                                "WHERE "
                                    "`tbl_derived2_id`=UuidToBin('X3d127bcc-abb9-11e8-98d0-529269fb1459X')"
                            ")");
    expect_query(mock,  "UPDATE "
                            "`tbl_derived2` "
                        "SET "
                            "`tbl_base_id`=UuidToBin('X3d127db6-abb9-11e8-98d0-529269fb1459X'), "
                            "`tbl_test2_id_test2_nullable`=UuidToBin('X3d1283a6-abb9-11e8-98d0-529269fb1459X'), "
                            "`tbl_test2_id_test2_ptr_u`=UuidToBin('Xec0f0aac-b8b9-11e8-96f8-529269fb1459X'), "
                            "`tbl_test2_id_test2_ptr_s`=UuidToBin(null) "
                        "WHERE "
                            "`tbl_derived2_id`=UuidToBin('X3d127bcc-abb9-11e8-98d0-529269fb1459X')",
                        result_affected_rows(1));
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

    derived2 d2;
    d2.id                       = uuid("3d127db6-abb9-11e8-98d0-529269fb1459");
    d2.name                     = "derived2";
    d2.derived2_id              = uuid("3d127bcc-abb9-11e8-98d0-529269fb1459");
    d2.test2_nullable           = test2 { };
    d2.test2_nullable->id       = uuid("3d1283a6-abb9-11e8-98d0-529269fb1459");
    d2.test2_nullable->u8_data  = 10;
    d2.test2_nullable->i8_data  = 11;
    d2.test2_nullable->u16_data = 12;
    d2.test2_nullable->i16_data = 13;
    d2.test2_ptr_u              = std::make_unique<test2>();
    d2.test2_ptr_u->u8_data     = 20;
    d2.test2_ptr_u->i8_data     = 21;
    d2.test2_ptr_u->u16_data    = 22;
    d2.test2_ptr_u->i16_data    = 23;

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);
    context.update(static_cast<base&>(d2));
}

TEST(CppHibernateTests, update_derived3)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "UPDATE "
                            "`tbl_base` "
                        "SET "
                            "`name`='Xderived3X' "
                        "WHERE "
                            "`tbl_base_id`=UuidToBin('X3d1288ce-abb9-11e8-98d0-529269fb1459X')",
                        result_affected_rows(1));
    expect_query(mock,  "DELETE FROM "
                            "`tbl_test2` "
                        "WHERE "
                            "`tbl_test2_id` IN ("
                                "SELECT "
                                    "`tbl_test2_id_test2_nullable` "
                                "FROM "
                                    "`tbl_derived2` "
                                "WHERE "
                                    "`tbl_derived2_id`=UuidToBin('X3d1287a2-abb9-11e8-98d0-529269fb1459X')"
                            ")");
    expect_query(mock,  "DELETE FROM "
                            "`tbl_test2` "
                        "WHERE "
                            "`tbl_test2_id` IN ("
                                "SELECT "
                                    "`tbl_test2_id_test2_ptr_u` "
                                "FROM "
                                    "`tbl_derived2` "
                                "WHERE "
                                    "`tbl_derived2_id`=UuidToBin('X3d1287a2-abb9-11e8-98d0-529269fb1459X')"
                            ")");
    expect_query(mock,  "DELETE FROM "
                            "`tbl_test2` "
                        "WHERE "
                            "`tbl_test2_id` IN ("
                                "SELECT "
                                    "`tbl_test2_id_test2_ptr_s` "
                                "FROM "
                                    "`tbl_derived2` "
                                "WHERE "
                                    "`tbl_derived2_id`=UuidToBin('X3d1287a2-abb9-11e8-98d0-529269fb1459X')"
                            ")");
    expect_query(mock,  "UPDATE "
                            "`tbl_derived2` "
                        "SET "
                            "`tbl_base_id`=UuidToBin('X3d1288ce-abb9-11e8-98d0-529269fb1459X'), "
                            "`tbl_test2_id_test2_nullable`=UuidToBin(null), "
                            "`tbl_test2_id_test2_ptr_u`=UuidToBin(null), "
                            "`tbl_test2_id_test2_ptr_s`=UuidToBin(null) "
                        "WHERE "
                            "`tbl_derived2_id`=UuidToBin('X3d1287a2-abb9-11e8-98d0-529269fb1459X')",
                        result_affected_rows(1));
    expect_query(mock,  "UPDATE "
                            "`tbl_derived3` "
                        "SET "
                            "`tbl_derived2_id`=UuidToBin('X3d1287a2-abb9-11e8-98d0-529269fb1459X') "
                        "WHERE "
                            "`tbl_derived3_id`=UuidToBin('X3d12866c-abb9-11e8-98d0-529269fb1459X')",
                        result_affected_rows(1));
    expect_query(mock,  "UPDATE "
                            "`tbl_test3` "
                        "SET "
                            "`tbl_test3_id_test3_list`=NULL, "
                            "`tbl_test3_index_test3_list`=0 "
                        "WHERE "
                            "`tbl_test3_id_test3_list`=UuidToBin('X3d12866c-abb9-11e8-98d0-529269fb1459X')");
    expect_query(mock,  "UPDATE "
                            "`tbl_test3` "
                        "SET "
                            "`tbl_derived3_id_test3_list`=UuidToBin('X3d12866c-abb9-11e8-98d0-529269fb1459X'), "
                            "`tbl_derived3_index_test3_list`='X0X', "
                            "`u32_data`='X100X', "
                            "`i32_data`='X101X', "
                            "`u64_data`='X102X', "
                            "`i64_data`='X103X' "
                        "WHERE "
                            "`tbl_test3_id`=UuidToBin('X3d1289f0-abb9-11e8-98d0-529269fb1459X')",
                        result_affected_rows(1));
    expect_query(mock,  "SELECT Uuid()",
                        result_used({
                            { "435bd976-b8c3-11e8-96f8-529269fb1459" }
                        }));
    expect_query(mock,  "INSERT INTO "
                            "`tbl_test3` "
                        "SET "
                            "`tbl_test3_id`=UuidToBin('X435bd976-b8c3-11e8-96f8-529269fb1459X'), "
                            "`tbl_derived3_id_test3_list`=UuidToBin('X3d12866c-abb9-11e8-98d0-529269fb1459X'), "
                            "`tbl_derived3_index_test3_list`='X1X', "
                            "`tbl_derived3_id_test3_vector`=UuidToBin(null), "
                            "`tbl_derived3_index_test3_vector`='X0X', "
                            "`u32_data`='X110X', "
                            "`i32_data`='X111X', "
                            "`u64_data`='X112X', "
                            "`i64_data`='X113X'",
                        result_affected_rows(1));
    expect_query(mock,  "DELETE FROM "
                            "`tbl_test3` "
                        "WHERE "
                            "(`tbl_derived3_id_test3_list` IS NULL) AND "
                            "(`tbl_derived3_id_test3_vector` IS NULL)");
    expect_query(mock,  "UPDATE "
                            "`tbl_test3` "
                        "SET "
                            "`tbl_test3_id_test3_vector`=NULL, "
                            "`tbl_test3_index_test3_vector`=0 "
                        "WHERE "
                            "`tbl_test3_id_test3_vector`=UuidToBin('X3d12866c-abb9-11e8-98d0-529269fb1459X')");
    expect_query(mock,  "UPDATE "
                            "`tbl_test3` "
                        "SET "
                            "`tbl_derived3_id_test3_vector`=UuidToBin('X3d12866c-abb9-11e8-98d0-529269fb1459X'), "
                            "`tbl_derived3_index_test3_vector`='X0X', "
                            "`u32_data`='X200X', "
                            "`i32_data`='X201X', "
                            "`u64_data`='X202X', "
                            "`i64_data`='X203X' "
                        "WHERE "
                            "`tbl_test3_id`=UuidToBin('X3d128eb4-abb9-11e8-98d0-529269fb1459X')",
                        result_affected_rows(1));
    expect_query(mock,  "SELECT Uuid()",
                        result_used({
                            { "1c0a3592-b8c4-11e8-96f8-529269fb1459" }
                        }));
    expect_query(mock,  "INSERT INTO "
                            "`tbl_test3` "
                        "SET "
                            "`tbl_test3_id`=UuidToBin('X1c0a3592-b8c4-11e8-96f8-529269fb1459X'), "
                            "`tbl_derived3_id_test3_list`=UuidToBin(null), "
                            "`tbl_derived3_index_test3_list`='X0X', "
                            "`tbl_derived3_id_test3_vector`=UuidToBin('X3d12866c-abb9-11e8-98d0-529269fb1459X'), "
                            "`tbl_derived3_index_test3_vector`='X1X', "
                            "`u32_data`='X210X', "
                            "`i32_data`='X211X', "
                            "`u64_data`='X212X', "
                            "`i64_data`='X213X'",
                        result_affected_rows(1));
    expect_query(mock,  "UPDATE "
                            "`tbl_test3` "
                        "SET "
                            "`tbl_derived3_id_test3_vector`=UuidToBin('X3d12866c-abb9-11e8-98d0-529269fb1459X'), "
                            "`tbl_derived3_index_test3_vector`='X2X', "
                            "`u32_data`='X220X', "
                            "`i32_data`='X221X', "
                            "`u64_data`='X222X', "
                            "`i64_data`='X223X' "
                        "WHERE "
                            "`tbl_test3_id`=UuidToBin('X3d129134-abb9-11e8-98d0-529269fb1459X')",
                        result_affected_rows(1));
    expect_query(mock,  "DELETE FROM "
                            "`tbl_test3` "
                        "WHERE "
                            "(`tbl_derived3_id_test3_list` IS NULL) AND "
                            "(`tbl_derived3_id_test3_vector` IS NULL)");
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

    derived3 d3;
    d3.id = uuid("3d1288ce-abb9-11e8-98d0-529269fb1459");
    d3.name = "derived3";
    d3.derived2_id = uuid("3d1287a2-abb9-11e8-98d0-529269fb1459");
    d3.derived3_id = uuid("3d12866c-abb9-11e8-98d0-529269fb1459");
    d3.test3_list.emplace_back();
    d3.test3_list.back().id = uuid("3d1289f0-abb9-11e8-98d0-529269fb1459");
    d3.test3_list.back().u32_data = 100;
    d3.test3_list.back().i32_data = 101;
    d3.test3_list.back().u64_data = 102;
    d3.test3_list.back().i64_data = 103;
    d3.test3_list.emplace_back();
    d3.test3_list.back().u32_data = 110;
    d3.test3_list.back().i32_data = 111;
    d3.test3_list.back().u64_data = 112;
    d3.test3_list.back().i64_data = 113;
    d3.test3_vector.emplace_back();
    d3.test3_vector.back().id = uuid("3d128eb4-abb9-11e8-98d0-529269fb1459");
    d3.test3_vector.back().u32_data = 200;
    d3.test3_vector.back().i32_data = 201;
    d3.test3_vector.back().u64_data = 202;
    d3.test3_vector.back().i64_data = 203;
    d3.test3_vector.emplace_back();
    d3.test3_vector.back().u32_data = 210;
    d3.test3_vector.back().i32_data = 211;
    d3.test3_vector.back().u64_data = 212;
    d3.test3_vector.back().i64_data = 213;
    d3.test3_vector.emplace_back();
    d3.test3_vector.back().id = uuid("3d129134-abb9-11e8-98d0-529269fb1459");
    d3.test3_vector.back().u32_data = 220;
    d3.test3_vector.back().i32_data = 221;
    d3.test3_vector.back().u64_data = 222;
    d3.test3_vector.back().i64_data = 223;

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);
    context.update(static_cast<derived2&>(d3));
}