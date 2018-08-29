#pragma once

#include <cpphibernate/misc.h>
#include <cpphibernate/config.h>
#include <cpphibernate/driver/mariadb/helper/context.h>
#include <cpphibernate/driver/mariadb/schema/tables.h>
#include <cpphibernate/driver/mariadb/schema/schema.fwd.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* schema_t */

    struct schema_t
    {
        std::string     schema_name;
        tables_t        tables;

        inline schema_t() = default;
        inline schema_t(const schema_t&) = delete;
        inline schema_t(schema_t&& other)
            : schema_name(std::move(other).schema_name)
            , tables     (std::move(other).tables)
            { update(); }

        void update ();
        void print  (std::ostream& os) const;

        const table_t& table(size_t dataset_id) const;

        /* CRUD */
        void init(const init_context& context) const;
    };

}
end_namespace_cpphibernate_driver_mariadb