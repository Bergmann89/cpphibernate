#include <cpphibernate/driver/mariadb.h>

#include "test_helper.h"
#include "test_schema.h"
#include "mariadb_mock.h"

using namespace ::testing;
using namespace ::cpphibernate;

TEST(CppHibernateTests, init)
{
    StrictMock<mariadb_mock> mock;

    expect_query(mock,  "START TRANSACTION");
    expect_query(mock,  "DROP DATABASE IF EXISTS `test`");
    expect_query(mock,  "CREATE SCHEMA IF NOT EXISTS `test` DEFAULT CHARACTER SET utf8");
    expect_query(mock,  "USE `test`");

    expect_query(mock,  "CREATE FUNCTION IF NOT EXISTS UuidToBin(_uuid CHAR(36))\n"
                        "   RETURNS BINARY(16)\n"
                        "   LANGUAGE SQL\n"
                        "   DETERMINISTIC\n"
                        "   CONTAINS SQL\n"
                        "   SQL SECURITY INVOKER\n"
                        "RETURN\n"
                        "   UNHEX(CONCAT(\n"
                        "       SUBSTR(_uuid, 25, 12),\n"
                        "       SUBSTR(_uuid, 20,  4),\n"
                        "       SUBSTR(_uuid, 15,  4),\n"
                        "       SUBSTR(_uuid, 10,  4),\n"
                        "       SUBSTR(_uuid,  1,  8)\n"
                        "   )\n"
                        ")");

    expect_query(mock,  "CREATE FUNCTION IF NOT EXISTS BinToUuid(_bin BINARY(16))\n"
                        "    RETURNS CHAR(36)\n"
                        "    LANGUAGE SQL\n"
                        "    DETERMINISTIC\n"
                        "    CONTAINS SQL\n"
                        "    SQL SECURITY INVOKER\n"
                        "RETURN\n"
                        "    LCASE(CONCAT_WS('-',\n"
                        "        HEX(SUBSTR(_bin, 13, 4)),\n"
                        "        HEX(SUBSTR(_bin, 11, 2)),\n"
                        "        HEX(SUBSTR(_bin,  9, 2)),\n"
                        "        HEX(SUBSTR(_bin,  7, 2)),\n"
                        "        HEX(SUBSTR(_bin,  1, 6))\n"
                        "   )\n"
                        ")");

    expect_query(mock,  "CREATE TABLE IF NOT EXISTS `tbl_test1`\n"
                        "(\n"
                        "    `tbl_test1_id` BINARY(16) NOT NULL,\n"
                        "    `str_data` VARCHAR(100) NOT NULL,\n"
                        "    `str64_data` VARCHAR(64) NOT NULL,\n"
                        "    `u32_nullable` INT UNSIGNED NULL DEFAULT NULL,\n"
                        "    `u32_ptr_u` INT UNSIGNED NULL DEFAULT NULL,\n"
                        "    `u32_ptr_s` INT UNSIGNED NULL DEFAULT NULL,\n"
                        "    PRIMARY KEY ( `tbl_test1_id` ),\n"
                        "    UNIQUE INDEX `index_tbl_test1_id` ( `tbl_test1_id` ASC )\n"
                        ")\n"
                        "ENGINE = InnoDB\n"
                        "DEFAULT CHARACTER SET = utf8");

    expect_query(mock,  "CREATE TABLE IF NOT EXISTS `tbl_test2`\n"
                        "(\n"
                        "    `tbl_test2_id` BINARY(16) NOT NULL,\n"
                        "    `u8_data` TINYINT UNSIGNED NOT NULL,\n"
                        "    `i8_data` TINYINT NOT NULL,\n"
                        "    `u16_data` SMALLINT UNSIGNED NOT NULL,\n"
                        "    `i16_data` SMALLINT NOT NULL,\n"
                        "    PRIMARY KEY ( `tbl_test2_id` ),\n"
                        "    UNIQUE INDEX `index_tbl_test2_id` ( `tbl_test2_id` ASC )\n"
                        ")\n"
                        "ENGINE = InnoDB\n"
                        "DEFAULT CHARACTER SET = utf8");

    expect_query(mock,  "CREATE TABLE IF NOT EXISTS `tbl_test3`\n"
                        "(\n"
                        "    `tbl_test3_id` BINARY(16) NOT NULL,\n"
                        "    `tbl_derived3_id_test3_list` BINARY(16) NULL DEFAULT NULL,\n"
                        "    `tbl_derived3_id_test3_vector` BINARY(16) NULL DEFAULT NULL,\n"
                        "    `u32_data` INT UNSIGNED NOT NULL,\n"
                        "    `i32_data` INT NOT NULL,\n"
                        "    `u64_data` BIGINT UNSIGNED NOT NULL,\n"
                        "    `i64_data` BIGINT NOT NULL,\n"
                        "    PRIMARY KEY ( `tbl_test3_id` ),\n"
                        "    UNIQUE INDEX `index_tbl_test3_id` ( `tbl_test3_id` ASC ),\n"
                        "    INDEX `index_tbl_derived3_id_test3_list` ( `tbl_derived3_id_test3_list` ASC ),\n"
                        "    INDEX `index_tbl_derived3_id_test3_vector` ( `tbl_derived3_id_test3_vector` ASC ),\n"
                        "    CONSTRAINT `fk_tbl_test3_tbl_derived3_id_test3_list`\n"
                        "        FOREIGN KEY (`tbl_derived3_id_test3_list`)\n"
                        "        REFERENCES `test`.`tbl_derived3` (`tbl_derived3_id`)\n"
                        "        ON DELETE SET NULL\n"
                        "        ON UPDATE NO ACTION,\n"
                        "    CONSTRAINT `fk_tbl_test3_tbl_derived3_id_test3_vector`\n"
                        "        FOREIGN KEY (`tbl_derived3_id_test3_vector`)\n"
                        "        REFERENCES `test`.`tbl_derived3` (`tbl_derived3_id`)\n"
                        "        ON DELETE SET NULL\n"
                        "        ON UPDATE NO ACTION\n"
                        ")\n"
                        "ENGINE = InnoDB\n"
                        "DEFAULT CHARACTER SET = utf8");

    expect_query(mock,  "CREATE TABLE IF NOT EXISTS `tbl_base`\n"
                        "(\n"
                        "    `tbl_base_id` BINARY(16) NOT NULL,\n"
                        "    `name` VARCHAR(100) NOT NULL,\n"
                        "    `__type` INT UNSIGNED NOT NULL,\n"
                        "    PRIMARY KEY ( `tbl_base_id` ),\n"
                        "    UNIQUE INDEX `index_tbl_base_id` ( `tbl_base_id` ASC )\n"
                        ")\n"
                        "ENGINE = InnoDB\n"
                        "DEFAULT CHARACTER SET = utf8");

    expect_query(mock,  "CREATE TABLE IF NOT EXISTS `tbl_derived1`\n"
                        "(\n"
                        "    `tbl_derived1_id` BINARY(16) NOT NULL,\n"
                        "    `tbl_base_id` BINARY(16) NOT NULL,\n"
                        "    `tbl_test1_id_test1_data` BINARY(16) NOT NULL,\n"
                        "    `enum_data` ENUM ( 'test0', 'test1', 'test2', 'test3' ) NOT NULL,\n"
                        "    PRIMARY KEY ( `tbl_derived1_id` ),\n"
                        "    UNIQUE INDEX `index_tbl_derived1_id` ( `tbl_derived1_id` ASC ),\n"
                        "    UNIQUE INDEX `index_tbl_base_id` ( `tbl_base_id` ASC ),\n"
                        "    INDEX `index_tbl_test1_id_test1_data` ( `tbl_test1_id_test1_data` ASC ),\n"
                        "    CONSTRAINT `fk_tbl_derived1_to_tbl_base_id`\n"
                        "        FOREIGN KEY (`tbl_base_id`)\n"
                        "        REFERENCES `test`.`tbl_base` (`tbl_base_id`)\n"
                        "        ON DELETE CASCADE\n"
                        "        ON UPDATE NO ACTION,\n"
                        "    CONSTRAINT `fk_tbl_derived1_to_tbl_test1_id_test1_data`\n"
                        "        FOREIGN KEY (`tbl_test1_id_test1_data`)\n"
                        "        REFERENCES `test`.`tbl_test1` (`tbl_test1_id`)\n"
                        "        ON DELETE CASCADE\n"
                        "        ON UPDATE NO ACTION\n"
                        ")\n"
                        "ENGINE = InnoDB\n"
                        "DEFAULT CHARACTER SET = utf8");

    expect_query(mock,  "CREATE TABLE IF NOT EXISTS `tbl_derived2`\n"
                        "(\n"
                        "    `tbl_derived2_id` BINARY(16) NOT NULL,\n"
                        "    `tbl_base_id` BINARY(16) NOT NULL,\n"
                        "    `tbl_test2_id_test2_nullable` BINARY(16) NULL DEFAULT NULL,\n"
                        "    `tbl_test2_id_test2_ptr_u` BINARY(16) NULL DEFAULT NULL,\n"
                        "    `tbl_test2_id_test2_ptr_s` BINARY(16) NULL DEFAULT NULL,\n"
                        "    PRIMARY KEY ( `tbl_derived2_id` ),\n"
                        "    UNIQUE INDEX `index_tbl_derived2_id` ( `tbl_derived2_id` ASC ),\n"
                        "    UNIQUE INDEX `index_tbl_base_id` ( `tbl_base_id` ASC ),\n"
                        "    INDEX `index_tbl_test2_id_test2_nullable` ( `tbl_test2_id_test2_nullable` ASC ),\n"
                        "    INDEX `index_tbl_test2_id_test2_ptr_u` ( `tbl_test2_id_test2_ptr_u` ASC ),\n"
                        "    INDEX `index_tbl_test2_id_test2_ptr_s` ( `tbl_test2_id_test2_ptr_s` ASC ),\n"
                        "    CONSTRAINT `fk_tbl_derived2_to_tbl_base_id`\n"
                        "        FOREIGN KEY (`tbl_base_id`)\n"
                        "        REFERENCES `test`.`tbl_base` (`tbl_base_id`)\n"
                        "        ON DELETE CASCADE\n"
                        "        ON UPDATE NO ACTION,\n"
                        "    CONSTRAINT `fk_tbl_derived2_to_tbl_test2_id_test2_nullable`\n"
                        "        FOREIGN KEY (`tbl_test2_id_test2_nullable`)\n"
                        "        REFERENCES `test`.`tbl_test2` (`tbl_test2_id`)\n"
                        "        ON DELETE CASCADE\n"
                        "        ON UPDATE NO ACTION,\n"
                        "    CONSTRAINT `fk_tbl_derived2_to_tbl_test2_id_test2_ptr_u`\n"
                        "        FOREIGN KEY (`tbl_test2_id_test2_ptr_u`)\n"
                        "        REFERENCES `test`.`tbl_test2` (`tbl_test2_id`)\n"
                        "        ON DELETE CASCADE\n"
                        "        ON UPDATE NO ACTION,\n"
                        "    CONSTRAINT `fk_tbl_derived2_to_tbl_test2_id_test2_ptr_s`\n"
                        "        FOREIGN KEY (`tbl_test2_id_test2_ptr_s`)\n"
                        "        REFERENCES `test`.`tbl_test2` (`tbl_test2_id`)\n"
                        "        ON DELETE CASCADE\n"
                        "        ON UPDATE NO ACTION\n"
                        ")\n"
                        "ENGINE = InnoDB\n"
                        "DEFAULT CHARACTER SET = utf8");

    expect_query(mock,  "CREATE TABLE IF NOT EXISTS `tbl_derived3`\n"
                        "(\n"
                        "    `tbl_derived3_id` BINARY(16) NOT NULL,\n"
                        "    `tbl_derived2_id` BINARY(16) NOT NULL,\n"
                        "    PRIMARY KEY ( `tbl_derived3_id` ),\n"
                        "    UNIQUE INDEX `index_tbl_derived3_id` ( `tbl_derived3_id` ASC ),\n"
                        "    UNIQUE INDEX `index_tbl_derived2_id` ( `tbl_derived2_id` ASC ),\n"
                        "    CONSTRAINT `fk_tbl_derived3_to_tbl_derived2_id`\n"
                        "        FOREIGN KEY (`tbl_derived2_id`)\n"
                        "        REFERENCES `test`.`tbl_derived2` (`tbl_derived2_id`)\n"
                        "        ON DELETE CASCADE\n"
                        "        ON UPDATE NO ACTION\n"
                        ")\n"
                        "ENGINE = InnoDB\n"
                        "DEFAULT CHARACTER SET = utf8");

    expect_query(mock,  "COMMIT");

    EXPECT_CALL(
        mock,
        mysql_close(
            reinterpret_cast<MYSQL*>(0x1111)));

    ::cppmariadb::connection connection(reinterpret_cast<MYSQL*>(0x1111));
    auto context = make_context<driver::mariadb>(test_schema, connection);
    context.init(true);
}