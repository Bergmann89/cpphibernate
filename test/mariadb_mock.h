#pragma once

#include <gmock/gmock.h>
#include <mariadb/errmsg.h>
#include <mariadb/mysqld_error.h>
#include <mariadb/mysql.h>

#define MARIADB_MOCK

#if !defined(_WIN32)
    #define STDCALL
#else
    #define STDCALL __stdcall
#endif

#define NOT_NULL_FLAG                   1       /* field can't be NULL */
#define PRI_KEY_FLAG                    2       /* field is part of a primary key */
#define UNIQUE_KEY_FLAG                 4       /* field is part of a unique key */
#define MULTIPLE_KEY_FLAG               8       /* field is part of a key */
#define BLOB_FLAG                       16      /* field is a blob */
#define UNSIGNED_FLAG                   32      /* field is unsigned */
#define ZEROFILL_FLAG                   64      /* field is zerofill */
#define BINARY_FLAG                     128
#define ENUM_FLAG                       256     /* field is an enum */
#define AUTO_INCREMENT_FLAG             512     /* field is a autoincrement field */
#define TIMESTAMP_FLAG                  1024    /* field is a timestamp */
#define SET_FLAG                        2048    /* field is a set */
#define NO_DEFAULT_VALUE_FLAG           4096    /* field doesn't have default value */
#define ON_UPDATE_NOW_FLAG              8192    /* field is set to NOW on UPDATE */
#define NUM_FLAG                        32768   /* field is num (for clients) */
#define PART_KEY_FLAG                   16384   /* Intern; Part of some key */
#define GROUP_FLAG                      32768   /* Intern: Group field */
#define UNIQUE_FLAG                     65536   /* Intern: Used by sql_yacc */


#define CLIENT_MYSQL                    1
#define CLIENT_FOUND_ROWS               2       /* Found instead of affected rows */
#define CLIENT_LONG_FLAG                4       /* Get all column flags */
#define CLIENT_CONNECT_WITH_DB          8       /* One can specify db on connect */
#define CLIENT_NO_SCHEMA                16      /* Don't allow database.table.column */
#define CLIENT_COMPRESS                 32      /* Can use compression protocol */
#define CLIENT_ODBC                     64      /* Odbc client */
#define CLIENT_LOCAL_FILES              128     /* Can use LOAD DATA LOCAL */
#define CLIENT_IGNORE_SPACE             256     /* Ignore spaces before '(' */
#define CLIENT_INTERACTIVE              1024    /* This is an interactive client */
#define CLIENT_SSL                      2048    /* Switch to SSL after handshake */
#define CLIENT_IGNORE_SIGPIPE           4096    /* IGNORE sigpipes */
#define CLIENT_TRANSACTIONS             8192    /* Client knows about transactions */
#define CLIENT_PROTOCOL_41              512
#define CLIENT_RESERVED                 16384
#define CLIENT_SECURE_CONNECTION        32768
#define CLIENT_MULTI_STATEMENTS         (1UL << 16)
#define CLIENT_MULTI_RESULTS            (1UL << 17)
#define CLIENT_PS_MULTI_RESULTS         (1UL << 18)
#define CLIENT_PLUGIN_AUTH              (1UL << 19)
#define CLIENT_CONNECT_ATTRS            (1UL << 20)
#define CLIENT_SESSION_TRACKING         (1UL << 23)
#define CLIENT_PROGRESS                 (1UL << 29)
#define CLIENT_PROGRESS_OBSOLETE        CLIENT_PROGRESS
#define CLIENT_SSL_VERIFY_SERVER_CERT   (1UL << 30)
#define CLIENT_REMEMBER_OPTIONS         (1UL << 31)

struct MariaDbMock
{
private:
    static void setInstance(MariaDbMock* value);
    static void clearInstance(MariaDbMock* value);

public:
    MOCK_METHOD1(mysql_num_rows,           my_ulonglong    (MYSQL_RES *res));
    MOCK_METHOD1(mysql_num_fields,         unsigned int    (MYSQL_RES *res));
    MOCK_METHOD1(mysql_row_tell,           MYSQL_ROWS*     (MYSQL_RES *res));
    MOCK_METHOD1(mysql_free_result,        void            (MYSQL_RES *res));
    MOCK_METHOD2(mysql_row_seek,           MYSQL_ROW_OFFSET(MYSQL_RES *res, MYSQL_ROW_OFFSET));
    MOCK_METHOD2(mysql_data_seek,          void            (MYSQL_RES *res, unsigned long long offset));
    MOCK_METHOD1(mysql_fetch_lengths,      unsigned long*  (MYSQL_RES *res));
    MOCK_METHOD1(mysql_fetch_row,          MYSQL_ROW       (MYSQL_RES *res));
    MOCK_METHOD1(mysql_fetch_fields,       MYSQL_FIELD*    (MYSQL_RES *res));

    MOCK_METHOD3(mysql_real_query,         int             (MYSQL *mysql, const char *q, unsigned long length));
    MOCK_METHOD1(mysql_errno,              unsigned int    (MYSQL *mysql));
    MOCK_METHOD1(mysql_error,              const char*     (MYSQL *mysql));
    MOCK_METHOD1(mysql_store_result,       MYSQL_RES*      (MYSQL *mysql));
    MOCK_METHOD1(mysql_use_result,         MYSQL_RES*      (MYSQL *mysql));
    MOCK_METHOD1(mysql_field_count,        unsigned int    (MYSQL *mysql));
    MOCK_METHOD1(mysql_affected_rows,      my_ulonglong    (MYSQL *mysql));
    MOCK_METHOD1(mysql_insert_id,          my_ulonglong    (MYSQL *mysql));
    MOCK_METHOD4(mysql_real_escape_string, unsigned long   (MYSQL *mysql, char *to, const char *from, unsigned long length));
    MOCK_METHOD1(mysql_close,              void            (MYSQL *mysql));
    MOCK_METHOD8(mysql_real_connect,       MYSQL*          (MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long clientflag));
    MOCK_METHOD1(mysql_init,               MYSQL*          (MYSQL *mysql));

    MariaDbMock()
        { setInstance(this); }

    ~MariaDbMock()
        { clearInstance(this); }
};

my_ulonglong        STDCALL mysql_num_rows          (MYSQL_RES *res);
unsigned int        STDCALL mysql_num_fields        (MYSQL_RES *res);
MYSQL_ROWS*         STDCALL mysql_row_tell          (MYSQL_RES *res);
void                STDCALL mysql_free_result       (MYSQL_RES *res);
MYSQL_ROW_OFFSET    STDCALL mysql_row_seek          (MYSQL_RES *res, MYSQL_ROW_OFFSET);
void                STDCALL mysql_data_seek         (MYSQL_RES *res, unsigned long long offset);
unsigned long*      STDCALL mysql_fetch_lengths     (MYSQL_RES *res);
MYSQL_ROW           STDCALL mysql_fetch_row         (MYSQL_RES *res);
MYSQL_FIELD*        STDCALL mysql_fetch_fields      (MYSQL_RES *res);

int                 STDCALL mysql_real_query        (MYSQL *mysql, const char *q, unsigned long length);
unsigned int        STDCALL mysql_errno             (MYSQL *mysql);
const char*         STDCALL mysql_error             (MYSQL *mysql);
MYSQL_RES*          STDCALL mysql_store_result      (MYSQL *mysql);
MYSQL_RES*          STDCALL mysql_use_result        (MYSQL *mysql);
unsigned int        STDCALL mysql_field_count       (MYSQL *mysql);
my_ulonglong        STDCALL mysql_affected_rows     (MYSQL *mysql);
my_ulonglong        STDCALL mysql_insert_id         (MYSQL *mysql);
unsigned long       STDCALL mysql_real_escape_string(MYSQL *mysql, char *to,const char *from, unsigned long length);
void                STDCALL mysql_close             (MYSQL *mysql);
MYSQL*              STDCALL mysql_real_connect      (MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long clientflag);
MYSQL*              STDCALL mysql_init              (MYSQL *mysql);