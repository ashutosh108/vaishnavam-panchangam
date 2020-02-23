#ifndef VP_CONCAT_H_INCLUDED
#define VP_CONCAT_H_INCLUDED

#include <string>
#include <sstream>

namespace vp {

template <typename... T>
std::string concat(T... args) {
    std::stringstream s;
    using DummyIntArray=int[];
    (void)DummyIntArray{0, ( (s << args), 0 ) ...};
    return s.str();
}

}

#endif // VP_CONCAT_H_INCLUDED
