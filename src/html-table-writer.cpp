#include "html-table-writer.h"

#include <string_view>

namespace vp {

Html_Table_Writer::Html_Table_Writer(const vp::Table & table) : table_(table){}

namespace {
std::string escape_html_attribute(const std::string & s) {
    std::string escaped;
    for (auto c : s) {
        switch(c) {
        case '&': escaped += "&amp;"; break;
        case '"': escaped += "&quot;"; break;
        case '\'': escaped += "&#039;"; break;
        case '<': escaped += "&lt;"; break;
        case '>': escaped += "&gt;"; break;
        default: escaped += c; break;
        }
    }
    return escaped;
}
}

std::ostream &operator<<(std::ostream &s, const Html_Table_Writer &tw)
{
    struct WriterCallBack : vp::Table::CallBack {
        std::ostream & s_;
        const vp::Table & table_;
        std::vector<bool> width_written_for_column;
        WriterCallBack(std::ostream & s, const vp::Table & table) : s_(s), table_(table){
            width_written_for_column.resize(table.width());
        }
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
        void write_col_width_if_needed(const vp::Table::Cell & cell) {
            if (width_written_for_column[cell.col]) {
                return;
            }
            auto & col_widths = table_.column_widths();
            if (col_widths.size() >= cell.col+1) {
                s_ << " width=\"" << col_widths[cell.col] << "%\"";
            }
            width_written_for_column[cell.col] = true;
        }
        void write_td_or_th(const vp::Table::Cell & cell, std::string_view tag) {
            s_ << "<" << tag;
            if (cell.rowspan != 1) {
                s_ << " rowspan=\"" << cell.rowspan << "\"";
            }
            if (cell.colspan != 1) {
                s_ << " colspan=\"" << cell.colspan << "\"";
            } else {
                write_col_width_if_needed(cell);
            }
            if (!cell.classes.empty()) {
                s_ << " class=\"" << cell.classes << "\"";
            }
            if (!cell.title.empty()) {
                s_ << " title=\"" << escape_html_attribute(cell.title) << "\"";
            }
            s_ << ">" << cell.text << "</" << tag << ">\n";
        }
        void cell(const vp::Table::Cell & cell) override {
            write_td_or_th(cell, "td");
        }
        void header_cell(const vp::Table::Cell & cell) override {
            write_td_or_th(cell, "th");
        }
    };

    s << "<table>\n";
    {
        WriterCallBack callback{s, tw.table_};
        tw.table_.iterate(callback);
    }
    s << "</table>\n";
    return s;
}

}
