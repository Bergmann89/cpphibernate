#include <cpphibernate/driver/mariadb.h>

#include "test_helper.h"
#include "test_schema.h"
#include "mariadb_mock.h"

using namespace ::testing;
using namespace ::cpphibernate;

TEST(CppHibernateTests, read_test1)
{
    StrictMock<mariadb_mock> mock;

    // expect_query(mock,  "START TRANSACTION");
    // expect_query(mock,  "COMMIT");

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
    t1.id = uuid("1e133ad8-ad2e-11e8-98d0-529269fb1459");

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);
    context.read(t1);
}