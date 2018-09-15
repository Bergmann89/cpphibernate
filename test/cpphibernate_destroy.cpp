#include <cpphibernate/driver/mariadb.h>

#include "test_helper.h"
#include "test_schema.h"
#include "mariadb_mock.h"

using namespace ::testing;
using namespace ::cpphibernate;

TEST(CppHibernateTests, destroy_test1)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "DELETE "
                            "`tbl_test1` "
                        "FROM "
                            "`tbl_test1` "
                        "WHERE "
                            "`tbl_test1`.`tbl_test1_id`=UuidToBin('X3d12697a-abb9-11e8-98d0-529269fb1459X')");
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
    t1.id = uuid("3d12697a-abb9-11e8-98d0-529269fb1459");

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);
    context.destroy(t1);
}

TEST(CppHibernateTests, destroy_test2)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "DELETE "
                            "`tbl_test2` "
                        "FROM "
                            "`tbl_test2` "
                        "WHERE "
                            "`tbl_test2`.`tbl_test2_id`=UuidToBin('X3d1270dc-abb9-11e8-98d0-529269fb1459X')");
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
    t2.id = uuid("3d1270dc-abb9-11e8-98d0-529269fb1459");

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);
    context.destroy(t2);
}

TEST(CppHibernateTests, destroy_test3)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "DELETE "
                            "`tbl_test3` "
                        "FROM "
                            "`tbl_test3` "
                        "WHERE "
                            "`tbl_test3`.`tbl_test3_id`=UuidToBin('X3d12737a-abb9-11e8-98d0-529269fb1459X')");
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
    t3.id = uuid("3d12737a-abb9-11e8-98d0-529269fb1459");

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);
    context.destroy(t3);
}

TEST(CppHibernateTests, destroy_derived1)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "DELETE "
                            "`tbl_base`, "
                            "`tbl_derived1`, "
                            "`T0` "
                        "FROM "
                            "`tbl_derived1` "
                        "LEFT JOIN "
                            "`tbl_base` ON `tbl_derived1`.`tbl_base_id`=`tbl_base`.`tbl_base_id` "
                        "LEFT JOIN "
                            "`tbl_test1` AS `T0` ON `tbl_derived1`.`tbl_test1_id_test1_data`=`T0`.`tbl_test1_id` "
                        "WHERE "
                            "`tbl_base`.`tbl_base_id`=UuidToBin('X3d12778a-abb9-11e8-98d0-529269fb1459X')");
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
    d1.id = uuid("3d12778a-abb9-11e8-98d0-529269fb1459");

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);
    context.destroy(static_cast<base&>(d1));
}

TEST(CppHibernateTests, destroy_derived2)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "DELETE "
                            "`tbl_base`, "
                            "`tbl_derived2`, "
                            "`T0`, "
                            "`T1`, "
                            "`T2`, "
                            "`tbl_derived3` "
                        "FROM "
                            "`tbl_derived2` "
                        "LEFT JOIN "
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
                            "`tbl_base`.`tbl_base_id`=UuidToBin('X3d127db6-abb9-11e8-98d0-529269fb1459X')");
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
    d2.id = uuid("3d127db6-abb9-11e8-98d0-529269fb1459");

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);
    context.destroy(static_cast<base&>(d2));
}

TEST(CppHibernateTests, destroy_derived3)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "DELETE "
                            "`tbl_base`, "
                            "`tbl_derived2`, "
                            "`T0`, "
                            "`T1`, "
                            "`T2`, "
                            "`tbl_derived3` "
                        "FROM "
                            "`tbl_derived3` "
                        "LEFT JOIN "
                            "`tbl_derived2` ON `tbl_derived3`.`tbl_derived2_id`=`tbl_derived2`.`tbl_derived2_id` "
                        "LEFT JOIN "
                            "`tbl_base` ON `tbl_derived2`.`tbl_base_id`=`tbl_base`.`tbl_base_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T0` ON `tbl_derived2`.`tbl_test2_id_test2_nullable`=`T0`.`tbl_test2_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T1` ON `tbl_derived2`.`tbl_test2_id_test2_ptr_u`=`T1`.`tbl_test2_id` "
                        "LEFT JOIN "
                            "`tbl_test2` AS `T2` ON `tbl_derived2`.`tbl_test2_id_test2_ptr_s`=`T2`.`tbl_test2_id` "
                        "WHERE "
                            "`tbl_derived2`.`tbl_derived2_id`=UuidToBin('X3d1287a2-abb9-11e8-98d0-529269fb1459X')");
    expect_query(mock,  "DELETE "
                            "`tbl_test3` "
                        "FROM "
                            "`tbl_test3` "
                        "WHERE "
                            "(`tbl_derived3_id_test3_list` IS NULL) AND "
                            "(`tbl_derived3_id_test3_vector` IS NULL)");
    expect_query(mock,  "DELETE "
                            "`tbl_test3` "
                        "FROM "
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
    d3.derived2_id = uuid("3d1287a2-abb9-11e8-98d0-529269fb1459");

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);
    context.destroy(static_cast<derived2&>(d3));
}