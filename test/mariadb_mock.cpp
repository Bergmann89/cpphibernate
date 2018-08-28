#include "mariadb_mock.h"

mariadb_mock* mariadb_mock_instance;

void mariadb_mock::setInstance(mariadb_mock* value)
{
    mariadb_mock_instance = value;
}

void mariadb_mock::clearInstance(mariadb_mock* value)
{
    if (mariadb_mock_instance == value)
        mariadb_mock_instance = nullptr;
}

my_ulonglong STDCALL mysql_num_rows (MYSQL_RES *res)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_num_rows(res) : 0); }

unsigned int STDCALL mysql_num_fields (MYSQL_RES *res)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_num_fields(res) : 0); }

MYSQL_ROWS* STDCALL mysql_row_tell (MYSQL_RES *res)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_row_tell(res) : nullptr); }

void STDCALL mysql_free_result (MYSQL_RES *res)
    { if (mariadb_mock_instance) mariadb_mock_instance->mysql_free_result(res); }

MYSQL_ROW_OFFSET STDCALL mysql_row_seek (MYSQL_RES *res, MYSQL_ROW_OFFSET offset)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_row_seek(res, offset) : nullptr); }

void STDCALL mysql_data_seek (MYSQL_RES *res, unsigned long long offset)
    { if (mariadb_mock_instance) mariadb_mock_instance->mysql_data_seek(res, offset); }

unsigned long* STDCALL mysql_fetch_lengths (MYSQL_RES *res)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_fetch_lengths(res) : nullptr); }

MYSQL_ROW STDCALL mysql_fetch_row (MYSQL_RES *res)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_fetch_row(res) : nullptr); }

MYSQL_FIELD* STDCALL mysql_fetch_fields (MYSQL_RES *res)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_fetch_fields(res) : nullptr); }

int STDCALL mysql_real_query (MYSQL *mysql, const char *q, unsigned long length)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_real_query(mysql, q, length) : 0); }

unsigned int STDCALL mysql_errno (MYSQL *mysql)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_errno(mysql) : 0); }

const char* STDCALL mysql_error (MYSQL *mysql)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_error(mysql) : nullptr); }

MYSQL_RES* STDCALL mysql_store_result (MYSQL *mysql)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_store_result(mysql) : nullptr); }

MYSQL_RES* STDCALL mysql_use_result (MYSQL *mysql)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_use_result(mysql) : nullptr); }

unsigned int STDCALL mysql_field_count (MYSQL *mysql)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_field_count(mysql) : 0); }

my_ulonglong STDCALL mysql_affected_rows (MYSQL *mysql)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_affected_rows(mysql) : 0); }

my_ulonglong STDCALL mysql_insert_id (MYSQL *mysql)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_insert_id(mysql) : 0); }

unsigned long STDCALL mysql_real_escape_string(MYSQL *mysql, char *to,const char *from, unsigned long length)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_real_escape_string(mysql, to, from, length) : 0); }

void STDCALL mysql_close (MYSQL *mysql)
    { if (mariadb_mock_instance) mariadb_mock_instance->mysql_close(mysql); }

MYSQL* STDCALL mysql_real_connect (MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long clientflag)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_real_connect(mysql, host, user, passwd, db, port, unix_socket, clientflag) : nullptr); }

MYSQL* STDCALL mysql_init (MYSQL *mysql)
    { return (mariadb_mock_instance ? mariadb_mock_instance->mysql_init(mysql) : nullptr); }