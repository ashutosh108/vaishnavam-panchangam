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
            return l.size() < r.size();
        }
    )->size();
}

int vp::Table::height() const
{
    return rows.size();
}

void vp::Table::add_cell(std::string text)
{
    if (rows.empty()) rows.emplace_back();
    rows.back().emplace_back(text);
}

void vp::Table::add_header_cell(std::string text)
{
    if (rows.empty()) rows.emplace_back();
    rows.back().emplace_back(text, true);
}

vp::Table::Cell &vp::Table::at(int row, int col)
{
    return rows.at(row).at(col);
}

const vp::Table::Cell &vp::Table::at(int row, int col) const
{
    return rows.at(row).at(col);
}

void vp::Table::iterate(vp::Table::CallBack &it)
{
    for ( auto & row : rows) {
        it.row_begin();
        for (auto & cell : row) {
            if (cell.is_header) {
                it.header_cell(cell.text);
            } else {
                it.cell(cell.text);
            }
        }
        it.row_end();
    }
}
