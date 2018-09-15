#include <string>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mariadb_mock.h"

ACTION(EscapeString)
{
    char*           dst = arg0;
    const char*     src = arg1;
    unsigned long   len = arg2;

    if (len <= 0)
        return 0;

    *(dst++) = 'X';
    for (unsigned long i = 0; i < len; ++i)
        *(dst++) = *(src++);
    *(dst++) = 'X';

    return len + 2;
}

struct result_data
    : public mariadb_mock_item
{
    using row_type  = std::vector<const char *>;
    using data_type = std::vector<row_type>;

    struct internal_data_t
    {
        std::vector<char*>          data;
        std::vector<unsigned long>  length;
    };
    using interal_data_vector = std::vector<internal_data_t>;

    bool                    is_stored;
    ssize_t                 affected_rows;
    data_type               data;
    interal_data_vector     internal_data;

    template<typename T_data>
    result_data(T_data&& p_data, bool p_is_stored, ssize_t p_affected_rows)
        : data          (std::forward<T_data>(p_data))
        , is_stored     (p_is_stored)
        , affected_rows (p_affected_rows)
    {
        internal_data.resize(data.size());
        for (size_t i = 0; i < data.size(); ++i)
        {
            auto& intern = internal_data.at(i);
            auto& d      = data.at(i);
            intern.data.resize(d.size());
            intern.length.resize(d.size());
            for (size_t j = 0; j < d.size(); ++j)
            {
                auto& str = d.at(j);
                intern.data[j]   = const_cast<char*>(str);
                intern.length[j] = static_cast<unsigned long>(str ? strlen(str) : 0);
            }
        }
    }
};

inline MYSQL_RES* next_result()
{
    static size_t value = 0x2000;
    return reinterpret_cast<MYSQL_RES*>(value++);
}

inline const result_data::data_type& empty_result_data()
{
    static const result_data::data_type value;
    return value;
}

template<typename T_data = decltype(empty_result_data())>
inline decltype(auto) result_stored(T_data&& data = empty_result_data(), ssize_t affected_rows = -1)
    { return result_data(std::forward<T_data>(data), true, affected_rows); }

template<typename T_data = decltype(empty_result_data())>
inline decltype(auto) result_used(T_data&& data = empty_result_data(), ssize_t affected_rows = -1)
    { return result_data(std::forward<T_data>(data), false, affected_rows); }

inline decltype(auto) result_affected_rows(ssize_t affected_rows)
    { return result_data(empty_result_data(), true, affected_rows); }

template<typename T_mock, typename T_result>
inline void expect_query(T_mock& mock, const std::string& query, T_result&& result)
{
    EXPECT_CALL(
        mock,
        mysql_real_query(
            reinterpret_cast<MYSQL*>(0x1111),
            ::testing::StrEq(query),
            query.size()))
        .InSequence(mock.sequence);

    auto& res = mock.store(std::forward<T_result>(result));
    auto  ptr = next_result();

    if (res.is_stored)
    {
        EXPECT_CALL(
            mock,
            mysql_store_result(reinterpret_cast<MYSQL*>(0x1111)))
                .InSequence(mock.sequence)
                .WillOnce(::testing::Return(ptr));
    }
    else
    {
        EXPECT_CALL(
            mock,
            mysql_use_result(reinterpret_cast<MYSQL*>(0x1111)))
                .InSequence(mock.sequence)
                .WillOnce(::testing::Return(ptr));
    }

    if (res.affected_rows >= 0)
    {
        EXPECT_CALL(
            mock,
            mysql_affected_rows(reinterpret_cast<MYSQL*>(0x1111)))
                .InSequence(mock.sequence)
                .WillOnce(::testing::Return(static_cast<unsigned long long>(res.affected_rows)));
    }

    if (!res.data.empty())
    {
        EXPECT_CALL(
            mock,
            mysql_num_fields(ptr))
                .Times(::testing::AnyNumber())
                .WillRepeatedly(::testing::Return(res.data.at(0).size()));
    }

    for (auto& x : res.internal_data)
    {
        EXPECT_CALL(
            mock,
            mysql_fetch_row(ptr))
                .InSequence(mock.sequence)
                .WillOnce(::testing::Return(x.data.data()));
        EXPECT_CALL(
            mock,
            mysql_fetch_lengths(ptr))
                .InSequence(mock.sequence)
                .WillOnce(::testing::Return(x.length.data()));
    }

    if (!res.is_stored)
    {
        EXPECT_CALL(
            mock,
            mysql_fetch_row(ptr))
                .InSequence(mock.sequence)
                .WillOnce(::testing::Return(nullptr));
    }

    EXPECT_CALL(
        mock,
        mysql_free_result(ptr))
        .InSequence(mock.sequence);
}

template<typename T_mock>
inline void expect_query(T_mock& mock, const std::string& query)
    { expect_query(mock, query, result_stored()); }