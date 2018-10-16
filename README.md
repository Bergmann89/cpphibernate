# cpphibernate

A C++17 library to serialize and deserialize C++ objects to a MYSQL/MARIADB database.

## Getting Started

### Prerequisites

Before you can use cpphibernate you need to download and install [the official mariadb C connector](https://downloads.mariadb.org/connector-c/)) and [the boost/hana library](https://github.com/boostorg/hana). And if you want to run the tests you additionally need [the google testing framework](https://github.com/google/googletest).

[The cpputils libary](https://git.bergmann89.de/cpp/cpputils) and [the cppmariadb library](https://github.com/Bergmann89/cppmariadb) will be automatically downladed during the build.

### Simple Usage Example

In this simple example we use a single and simple object, that is stored in the database. The create, update, read and destroy methods and the corresponding SQL queries are shown in the code below.

```cpp
#include <memory>
#include <iostream>
#include <exception>
#include <cpphibernate.h>
#include <cpphibernate/driver/mariadb.h>

/* define the class to hibernate */

struct test_data
{
    size_t                      id;         //!< ID of the class instance (this field is mandatory for the hibernate engine)
    std::string                 str_data;   //!< Add string data field
    ::cpphibernate::string<64>  str64_data; //!< Add a special string data field with a mex length of 64

    uint32_t                    u32;        //!< Add a normal integer data field
    std::unique_ptr<uint32_t>   u32_ptr_u;  //!< Add a nullable integer data field
};

/* create the database schema */

constexpr decltype(auto) test_schema = cpphibernate_make_schema(
    test_schema,                                                // this is the schema name
    cpphibernate_make_table_name(
        tbl_test_data,                                          // this is the table name
        test_data,                                              // this is the referenced class
        1,                                                      // a unique id for the table
        cpphibernate_make_id          (&test_data::id),         // pointer to the ID member
        cpphibernate_make_field       (test_data, str_data),    // define a normal member field
        cpphibernate_make_field       (test_data, str64_data),  // [...]
        cpphibernate_make_field       (test_data, u32),         // [...]
        cpphibernate_make_field       (test_data, u32_ptr_u),   // [...]
    )
);

int main(int argc, char** argv)
{
    try
    {
        using namespace ::cppmariadb;
        using namespace ::cpphibernate;

        /* establish connection to database */
        connection c = database::connect("localhost", 3306, "testuser", "password", "", client_flags::empty());

        /* create a hibernation context */
        auto context = make_context_ptr<driver::mariadb>(test_schema, c);

        /* initialize the database schema */
        context.init(); /*          CREATE SCHEMA IF NOT EXISTS `test_schema` DEFAULT CHARACTER SET utf8;
                                    USE `test_schema`;
                                    CREATE TABLE IF NOT EXISTS `tbl_test_data`
                                    (
                                        `tbl_test_data_id` INT UNSIGNED NOT NULL,
                                        `str_data` VARCHAR(100) NOT NULL,
                                        `str64_data` VARCHAR(64) NOT NULL,
                                        `u32` INT UNSIGNED NOT NULL,
                                        `u32_ptr_u` INT UNSIGNED NULL DEFAULT NULL,
                                        PRIMARY KEY ( `tbl_test_data_id` ),
                                        UNIQUE INDEX `index_tbl_test_data_id` ( `tbl_test_data_id` ASC )
                                    )
                                    ENGINE = InnoDB
                                    DEFAULT CHARACTER SET = utf8; */

        /* create some test data */
        test_data data;
        data.str_data   = "this is a simple string";
        data.str64_data = "this is a string with max 64 characters";
        data.u32        = 123;

        /* create a new dataset in the database:
         * the new IDs of the object are stored in the corresponding members */
        context.create(data); /*    INSERT INTO
                                        `tbl_test_data`
                                    SET
                                        `str_data`='this is a simple string',
                                        `str64_data`='this is a string with max 64 characters',
                                        `u32`=123,
                                        `u32_ptr_u`=NULL; */

        /* change some data and update the database */
        t1.u32_ptr_u.reset(new uint32_t(456));
        context.update(data); /*    UPDATE
                                        `tbl_test_data`
                                    SET
                                        `str_data`='this is a simple string',
                                        `str64_data`='this is a string with max 64 characters',
                                        `u32`=123,
                                        `u32_ptr_u`=456
                                    WHERE
                                        `tbl_test_data_id`=1; */

        /* read back the created object:
         * if no selector is passed here, the ID member of the object is used */
        context.read(data); /*      SELECT "
                                        `tbl_test_data`.`tbl_test_data_id`),
                                        `tbl_test_data`.`str_data`,
                                        `tbl_test_data`.`str64_data`,
                                        `tbl_test_data`.`u32`,
                                        `tbl_test_data`.`u32_ptr_u`
                                    FROM
                                        `tbl_test_data`
                                    WHERE
                                        `tbl_test_data`.`tbl_test_data_id`=1; */

        /* delete the object from the database */
        context.destroy(data); /*   DELETE
                                        `tbl_test_data`
                                    FROM
                                        `tbl_test_data`
                                    WHERE
                                        `tbl_test_data`.`tbl_test_data_id`=1; */

        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return 1
}
```

### More Advanced Usage Example

The more advanced example uses a more complex database schema, containing custom defined enum types, one to one and one to many correleation of different tables, a polymorphic class hierarchy with inheritance and some of the std containers like std::unique_ptr or std::vector.

```cpp
#include <memory>
#include <iostream>
#include <exception>
#include <cpphibernate.h>
#include <cpphibernate/driver/mariadb.h>
#include <cpputils/misc/enum.h>
#include <cpputils/container/nullable.h>

/* define a test enum */

enum class test_enum
{
    test0,
    test1,
    test2,
    test3,

    first = test0,
    last  = test3,
};

/* define the enum to string mapping for the test enum */

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

/* define some normal C++ structs/classes */

struct test1
{
    size_t                      id;
    std::string                 str_data;
    ::cpphibernate::string<64>  str64_data;

    utl::nullable<uint32_t>     u32_nullable;
    std::unique_ptr<uint32_t>   u32_ptr_u;
    std::shared_ptr<uint32_t>   u32_ptr_s;
};

struct test2
{
    size_t                      id;
    uint8_t                     u8_data;
    int8_t                      i8_data;
    uint16_t                    u16_data;
    int16_t                     i16_data;
};

struct test3
{
    size_t                      id;
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

/* create the database schema */

constexpr decltype(auto) test_schema = cpphibernate_make_schema(
    test,                                                       // this is the schema name
    cpphibernate_make_table_name(
        tbl_test1,                                              // this is the table name
        test1,                                                  // this is the referenced class
        1,                                                      // a unique id for the table
        cpphibernate_make_id          (&test1::id),             // pointer to the ID member
        cpphibernate_make_field       (test1, str_data),        // define a normal member field
        cpphibernate_make_field       (test1, str64_data),      // [...]
        cpphibernate_make_field       (test1, u32_nullable),    // [...]
        cpphibernate_make_field       (test1, u32_ptr_u),       // [...]
        cpphibernate_make_field       (test1, u32_ptr_s)        // [...]
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

int main(int argc, char** argv)
{
    try
    {
        using namespace ::cppmariadb;
        using namespace ::cpphibernate;

        /* establish connection to database */
        connection c = database::connect("localhost", 3306, "testuser", "password", "", client_flags::empty());

        /* create a hibernation context */
        auto context = make_context_ptr<driver::mariadb>(test_schema, c);

        /* initialize the database schema */
        context.init();

        /* create some test data */
        derived3 d3;
        d3.name = "derived3";
        d3.test3_list.emplace_back();
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
        d3.test3_vector.back().u32_data = 220;
        d3.test3_vector.back().i32_data = 221;
        d3.test3_vector.back().u64_data = 222;
        d3.test3_vector.back().i64_data = 223;

        /* create a new dataset in the database:
         * the new IDs of the object are stored in the corresponding members */
        context.create(d3);

        /* read back the created object:
         * if no selector is passed here, the ID member of the object is used */
        context.read(d3);

        /* if we pass a pointer to the read method, the suitable object is created automatically */
        using namespace ::boost::hana::literals;
        using namespace ::cpphibernate::modifier;
        using base_ptr_u = std::unique_ptr<base>;

        base_ptr_u b;
        constexpr decltype(auto) base_key_field = test_schema.tables[3_c].fields[0_c];

        context.read(b, where(equal(base_key_field, d3.id)));
        auto * d3_ptr = dynamic_cast<derived3*>(b.get());
        if (d3_ptr)
        {
            /* do something with the data */
        }

        /* this also works for containers of pointers */
        using base_vector = std::vector<base_ptr_u>;
        base_vector vec;
        context.read(vec);
        for (auto& ptr : vec)
        {
            /* do something with the data */
        }

        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return 1
}
```

## License

This project is licensed under the MIT License - see the [LICENSE.txt](LICENSE.txt) file for details