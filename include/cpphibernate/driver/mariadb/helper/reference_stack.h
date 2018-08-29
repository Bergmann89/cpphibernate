#pragma once

#include <stack>

#include <cpphibernate/config.h>
#include <cpputils/misc/type_helper.h>

beg_namespace_cpphibernate_driver_mariadb
{

    /* reference_lock */

    struct reference_lock
    {
        virtual ~reference_lock() = default;
    };

    using reference_lock_ptr = std::unique_ptr<reference_lock>;

    /* reference_stack */

    template<typename T_dataset>
    struct reference_stack
    {
    private:
        struct lock
            : public reference_lock
        {
        private:
            T_dataset& dataset;

        public:
            inline lock(T_dataset& p_dataset)
                : dataset(p_dataset)
                { push_impl(dataset); }

            virtual ~lock() override
                { pop_impl(dataset); }
        };

    private:
        using stack_type = std::stack<T_dataset*>;

        static inline stack_type& stack()
        {
            static stack_type value;
            return value;
        }

        static inline void push_impl(T_dataset& dataset)
            { stack().push(&dataset); }

        static inline void pop_impl(T_dataset& dataset)
        {
            if (stack().empty() || stack().top() != &dataset)
                throw misc::hibernate_exception(std::string("reference_stack<") + utl::type_helper<T_dataset>::name() + ">: poped element is not the top element!");
            stack().pop();
        }

    public:
        static inline decltype(auto) push(T_dataset& dataset)
            { return std::make_unique<lock>(dataset); }

        static inline T_dataset& top()
        {
            if (stack().empty())
                throw misc::hibernate_exception(std::string("reference_stack<") + utl::type_helper<T_dataset>::name() + ">: does not have stored a dataset!");
            return *stack().top();
        }

        static inline size_t size()
            { return stack().size(); }
    };

}
end_namespace_cpphibernate_driver_mariadb