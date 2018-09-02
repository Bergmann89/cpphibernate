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
                            "`tbl_test1`.`tbl_test1_id`, "
                            "`tbl_test1`.`str_data`, "
                            "`tbl_test1`.`str64_data`, "
                            "`tbl_test1`.`u32_nullable`, "
                            "`tbl_test1`.`u32_ptr_u`, "
                            "`tbl_test1`.`u32_ptr_s` "
                        "FROM "
                            "`tbl_test1`   ",
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
                            "`tbl_test2`.`tbl_test2_id`, "
                            "`tbl_test2`.`u8_data`, "
                            "`tbl_test2`.`i8_data`, "
                            "`tbl_test2`.`u16_data`, "
                            "`tbl_test2`.`i16_data` "
                        "FROM "
                            "`tbl_test2`   ",
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

    EXPECT_EQ(1, t2.u8_data);
    EXPECT_EQ(2, t2.i8_data);
    EXPECT_EQ(3, t2.u16_data);
    EXPECT_EQ(4, t2.i16_data);
}

TEST(CppHibernateTests, read_test3)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "SELECT "
                            "`tbl_test3`.`tbl_test3_id`, "
                            "`tbl_test3`.`u32_data`, "
                            "`tbl_test3`.`i32_data`, "
                            "`tbl_test3`.`u64_data`, "
                            "`tbl_test3`.`i64_data` "
                        "FROM "
                            "`tbl_test3`   ",
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

    EXPECT_EQ(5, t3.u32_data);
    EXPECT_EQ(6, t3.i32_data);
    EXPECT_EQ(7, t3.u64_data);
    EXPECT_EQ(8, t3.i64_data);
}
