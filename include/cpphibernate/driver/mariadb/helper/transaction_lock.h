#pragma once

#include <memory>
#include <iomanip>
#include <cppmariadb.h>
#include <cpphibernate/config.h>
#include <cpputils/misc/type_helper.h>

beg_namespace_cpphibernate_driver_mariadb
{

    struct transaction_lock final
    {
    public:
        inline transaction_lock(::cppmariadb::connection& con)
            { begin_transaction(*this, con); }

        inline ~transaction_lock()
            { close_transaction(*this); }

        inline bool commit()
            { return commit_transaction(*this); }

    private:
        using transaction_ptr_type = std::unique_ptr<::cppmariadb::transaction>;

#ifdef cpphibernate_debug
#   define  debug_log(str)  cpphibernate_debug_log(                                                             \
                                "transaction (id=" << std::setw(8) << std::setfill(' ') << lock.id <<           \
                                ", counter="       << std::setw(2) << std::setfill(' ') << counter << ") " str)
        struct counter { };
        size_t id { utl::unique_counter<counter>::next() };
#else
#   define  debug_log(str) do {  } while(0)
#endif

        static size_t& ref_counter()
        {
            static size_t value = 0;
            return value;
        }

        static transaction_ptr_type& ref_transaction_ptr()
        {
            static transaction_ptr_type value;
            return value;
        }

        static void begin_transaction(const transaction_lock& lock, ::cppmariadb::connection& con)
        {
            auto& counter = ref_counter();
            ++counter;
            debug_log("+++");
            if (counter == 1)
            {
                debug_log("begin");
                ref_transaction_ptr().reset(new ::cppmariadb::transaction(con));
            }
        }

        static bool commit_transaction(const transaction_lock& lock)
        {
            auto& counter = ref_counter();
            if (counter == 1)
            {
                debug_log("commit");
                ref_transaction_ptr()->commit();
                return true;
            }
            return false;
        }

        static void close_transaction(const transaction_lock& lock)
        {
            auto& counter = ref_counter();
            debug_log("---");
            if (counter <= 1)
            {
                debug_log("close");
                counter = 0;
                ref_transaction_ptr().reset();
            }
            else
            {
                --counter;
            }
        }
    };

#undef debug_log

}
end_namespace_cpphibernate_driver_mariadb