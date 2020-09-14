#include "table.h"

#include <algorithm>

vp::Table::Table()
{

}

int vp::Table::width() const
{
    if (rows.empty()) return 0;
    return std::max_element(
        rows.cbegin(),
        rows.cend(),
        [](const auto & l, const auto & r) {
            return l.data.size() < r.data.size();
        }
    )->data.size();
}

int vp::Table::height() const
{
    return rows.size();
}

void vp::Table::add_cell(std::string text, std::string classes)
{
    if (rows.empty()) rows.emplace_back();
    rows.back().data.emplace_back(text, false, std::move(classes));
}

void vp::Table::add_cell(std::string_view text, std::string classes)
{
    add_cell(std::string{text}, std::move(classes));
}

void vp::Table::add_cell(const char * const text, std::string classes) {
    add_cell(std::string{text}, std::move(classes));
}


void vp::Table::add_header_cell(std::string text, std::string classes)
{
    if (rows.empty()) rows.emplace_back();
    rows.back().data.emplace_back(text, true, std::move(classes));
}

vp::Table::Cell &vp::Table::at(int row, int col)
{
    return rows.at(row).data.at(col);
}

const vp::Table::Cell &vp::Table::at(int row, int col) const
{
    return rows.at(row).data.at(col);
}

void vp::Table::start_new_row(std::string classes)
{
    rows.emplace_back(classes);
}

void vp::Table::iterate(vp::Table::CallBack &it) const
{
    for ( auto & row : rows) {
        it.row_begin(row.classes);
        for (auto & cell : row.data) {
            if (cell.is_header) {
                it.header_cell(cell.text, cell.classes);
            } else {
                it.cell(cell.text, cell.classes);
            }
        }
        it.row_end();
    }
}
