#ifndef HTML_TABLE_WRITER_H
#define HTML_TABLE_WRITER_H

#include <ostream>
#include "table.h"

namespace vp {

class Html_Table_Writer
{
public:
    Html_Table_Writer(const vp::Table & table);
    friend std::ostream & operator<<(std::ostream & s, const vp::Html_Table_Writer & tw);
private:
    const vp::Table & table_;
};

std::ostream & operator<<(std::ostream & s, const vp::Html_Table_Writer & tw);

}

#endif // HMTL_TABLE_WRITER_H
