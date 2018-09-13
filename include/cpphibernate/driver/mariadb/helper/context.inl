#pragma once

#include <cpphibernate/driver/mariadb/helper/context.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* data_context */

    template<typename T_dataset>
    inline void* data_context
        ::set(T_dataset& dataset, size_t dataset_id) const
    {
        using dataset_type = mp::decay_t<T_dataset>;

        _table      = nullptr;
        _dataset    = &dataset;
        _dataset_id = (dataset_id == 0)
            ? misc::get_type_id(hana::type_c<dataset_type>)
            : dataset_id;

        return _dataset;
    }

    template<typename T_dataset>
    inline decltype(auto) data_context
        ::get() const
    {
        using dataset_type = mp::decay_t<T_dataset>;

        if (!_dataset)
            throw misc::hibernate_exception("no data assigned!");

        auto dataset_id = misc::get_type_id(hana::type_c<dataset_type>);
        if (dataset_id != _dataset_id)
        {
            /* check table */
            if (!_table)
                _table = &schema.table(_dataset_id);
            else if (_table->dataset_id != _dataset_id)
                throw misc::hibernate_exception("invalid table!");

            auto table = _table;
            while(table && table->dataset_id != dataset_id)
                table = table->base_table;

            if (!table)
            {
                throw misc::hibernate_exception(utl::type_helper<dataset_type>::name() +
                    " is not a derived type of dataset with id " + std::to_string(_dataset_id));
            }
        }
        return *static_cast<dataset_type*>(_dataset);
    }

    /* read_context */

    template<typename T_dataset>
    T_dataset& read_context
        ::emplace(const table_t* table) const
    {
        using dataset_type = mp::decay_t<T_dataset>;

        // check table
        auto dataset_id = misc::get_type_id(hana::type_c<dataset_type>);
        if (!table)
            table = &schema.table(dataset_id);
        else if (table->dataset_id != dataset_id)
            throw misc::hibernate_exception("wrong table passed!");

        // check base
        auto tbl = table;
        while (tbl && tbl->dataset_id != base_dataset_id)
            tbl = tbl->base_table;
        if (!tbl)
        {
            throw misc::hibernate_exception(utl::type_helper<dataset_type>::name() +
                " is not a derived type of dataset with id " + std::to_string(base_dataset_id));
        }

        // create dataset
        auto ptr  = std::make_unique<dataset_type>();
        auto data = emplace_intern(ptr.get(), dataset_id);
        if (!data)
            throw misc::hibernate_exception("unable to store created dataset in context!");
        ptr.release();
        return *static_cast<dataset_type*>(data);
    }

}
end_namespace_cpphibernate_driver_mariadb