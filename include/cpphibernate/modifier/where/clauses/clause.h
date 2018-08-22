#pragma once

#include <cpphibernate/config.h>

beg_namespace_cpphibernate_modifier
{

    namespace __impl
    {

        /* where_clause_t */

        struct where_clause_t
            { };

    }

    /* meta */

    template<typename T>
    struct is_where_clause
        : public mp::is_base_of<__impl::where_clause_t, T>
        { };

    template<typename... T>
    struct all_are_where_clauses
        : public mp::all_true<is_where_clause<T>::value...>
        { };

}
end_namespace_cpphibernate_modifier