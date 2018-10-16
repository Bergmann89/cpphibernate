#include <iomanip>

#include <cpphibernate/types.h>

using namespace ::cpphibernate;;

void uuid::to_string(std::ostream& os) const
{
    os  << std::setw(2) << std::setfill('0') << std::hex << (int)(*this)[ 0]
        << std::setw(2) << std::setfill('0') << std::hex << (int)(*this)[ 1]
        << std::setw(2) << std::setfill('0') << std::hex << (int)(*this)[ 2]
        << std::setw(2) << std::setfill('0') << std::hex << (int)(*this)[ 3]
        << '-'
        << std::setw(2) << std::setfill('0') << std::hex << (int)(*this)[ 4]
        << std::setw(2) << std::setfill('0') << std::hex << (int)(*this)[ 5]
        << '-'
        << std::setw(2) << std::setfill('0') << std::hex << (int)(*this)[ 6]
        << std::setw(2) << std::setfill('0') << std::hex << (int)(*this)[ 7]
        << '-'
        << std::setw(2) << std::setfill('0') << std::hex << (int)(*this)[ 8]
        << std::setw(2) << std::setfill('0') << std::hex << (int)(*this)[ 9]
        << '-'
        << std::setw(2) << std::setfill('0') << std::hex << (int)(*this)[10]
        << std::setw(2) << std::setfill('0') << std::hex << (int)(*this)[11]
        << std::setw(2) << std::setfill('0') << std::hex << (int)(*this)[12]
        << std::setw(2) << std::setfill('0') << std::hex << (int)(*this)[13]
        << std::setw(2) << std::setfill('0') << std::hex << (int)(*this)[14]
        << std::setw(2) << std::setfill('0') << std::hex << (int)(*this)[15];
}

bool uuid::from_string(const std::string& str, uuid& val)
{
    const char* c = str.data();
    const char* e = c + str.size();
    size_t i = 0;
    val.fill(0);
    while(i < 32 && c < e)
    {
        if (*c >= '0' && *c <= '9')
            val[i >> 1] = static_cast<uint8_t>(val[i >> 1] | ((*c - '0' +  0) << (4 * (1 - (i & 1)))));
        else if (*c >= 'a' && *c <= 'f')
            val[i >> 1] = static_cast<uint8_t>(val[i >> 1] | ((*c - 'a' + 10) << (4 * (1 - (i & 1)))));
        else if (*c >= 'A' && *c <= 'F')
            val[i >> 1] = static_cast<uint8_t>(val[i >> 1] | ((*c - 'A' + 10) << (4 * (1 - (i & 1)))));
        else if (*c != '-')
            return false;
        if (*c != '-')
            ++i;
        ++c;
    }
    if (i != 32 || c != e)
        return false;
    return true;
}