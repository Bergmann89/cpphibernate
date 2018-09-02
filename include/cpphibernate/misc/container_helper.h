#pragma once

#include <set>
#include <list>
#include <vector>

#include <cpphibernate/config.h>

beg_namespace_cpphibernate_misc
{

    /* container_helper */

    template<typename T_container, typename = void>
    struct container_helper
    {
        using container_type = T_container;
        using value_type     = real_dataset_t<container_type>;
    };

}
end_namespace_cpphibernate_misc