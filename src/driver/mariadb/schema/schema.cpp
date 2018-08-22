#include <string>
#include <iostream>
#include <cpphibernate/driver/mariadb/schema/schema.h>

using namespace ::cpphibernate::driver::mariadb_impl;

void schema_t::update()
{

}

void schema_t::print(std::ostream& os) const
{
    os << "fuu" << std::endl;
}