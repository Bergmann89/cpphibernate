#pragma once

#include <cpphibernate/driver/mariadb/helper/context.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* data_context */

    template<typename T>
    inline decltype(auto) data_context
        ::get(const table_t* table) const
    {
        if (!data)
            throw misc::hibernate_exception("no data assigned!");

        auto type_id = misc::get_type_id(hana::type_c<mp::decay_t<T>>);
        if (type_id != data_id)
        {
            if (!table)
                table = &schema.table(type_id);

            while(table && table->dataset_id != type_id)
                table = table->base_table;

            if (!table)
            {
                throw misc::hibernate_exception(static_cast<std::ostringstream&>(std::ostringstream { }
                    << "invalid type! expected " << data_id << ", got " << type_id).str());
            }
        }
        return *static_cast<T*>(data);
    }

    /* read_context */

    template<typename T_dataset>
    T_dataset& read_context
        ::emplace(const table_t* table) const
    {
        if (!is_dynamic || base_dataset_id == 0)
            throw misc::hibernate_exception("dynamic creation is deactivated for this context!");

        // check table
        auto dataset_id = misc::get_type_id(hana::type_c<mp::decay_t<T_dataset>>);
        if (!table)
            table = &schema.table(dataset_id);
        else if (table->dataset_id != dataset_id)
            throw misc::hibernate_exception("dataset id of table and dataset to insert defer!");

        // check base
        auto tbl = table;
        while (tbl && tbl->dataset_id != base_dataset_id)
            tbl = tbl->base_table;
        if (!tbl)
            throw misc::hibernate_exception(utl::type_helper<T_dataset>::name() + " is not a derived type of table " + table->table_name);

        // create dataset
        auto ptr = std::make_unique<T_dataset>();
        auto ret = emplace_intern(ptr.get());
        if (!ret)
            ret = ptr.release();
        return *static_cast<T_dataset*>(ret);
    }

}
end_namespace_cpphibernate_driver_mariadb