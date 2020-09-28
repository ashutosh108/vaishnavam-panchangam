#include "html-table-writer.h"

#include <string_view>

namespace vp {

Html_Table_Writer::Html_Table_Writer(const vp::Table & table) : table_(table){}

std::ostream &operator<<(std::ostream &s, const Html_Table_Writer &tw)
{
    struct WriterCallBack : vp::Table::CallBack {
        std::ostream & s_;
        WriterCallBack(std::ostream & s) : s_(s){}
        void row_begin(std::string_view classes) override {
            s_ << "<tr";
            if (!classes.empty()) {
                s_ << " class=\"" << classes << "\"";
            }
            s_ << ">";
        }
        void row_end() override {
            s_ << "</tr>\n";
        }
        void cell(const vp::Table::Cell & cell) override {
            s_ << "<td";
            if (cell.rowspan != 1) {
                s_ << " rowspan=\"" << cell.rowspan << "\"";
            }
            if (cell.colspan != 1) {
                s_ << " colspan=\"" << cell.colspan << "\"";
            }
            if (!cell.classes.empty()) {
                s_ << " class=\"" << cell.classes << "\"";
            }
            s_ << ">" << cell.text << "</td>\n";
        }
        void header_cell(const vp::Table::Cell & cell) override {
            s_ << "<th";
            if (cell.rowspan != 1) {
                s_ << " rowspan=\"" << cell.rowspan << "\"";
            }
            if (cell.colspan != 1) {
                s_ << " colspan=\"" << cell.colspan << "\"";
            }
            if (!cell.classes.empty()) {
                s_ << " class=\"" << cell.classes << "\"";
            }
            s_ << ">" << cell.text << "</th>\n";
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
