#include "html-table-writer.h"

#include <string_view>

namespace vp {

Html_Table_Writer::Html_Table_Writer(const vp::Table & table) : table_(table){}

std::ostream &operator<<(std::ostream &s, const Html_Table_Writer &tw)
{
    struct WriterCallBack : vp::Table::CallBack {
        std::ostream & s_;
        WriterCallBack(std::ostream & s) : s_(s){}
        void row_begin() override {
            s_ << "<tr>";
        }
        void row_end() override {
            s_ << "</tr>\n";
        }
        void cell(std::string_view text) override {
            s_ << "<td>" << text << "</td>\n";
        }
        void header_cell(std::string_view text) override {
            s_ << "<th>" << text << "</th>\n";
        }
    };

    s << "<table>\n";
    {
        WriterCallBack callback{s};
        tw.table_.iterate(callback);
    }
    s << "</table>\n";
    return s;
}

}
