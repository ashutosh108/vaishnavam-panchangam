#include "table.h"

#include <algorithm>

size_t vp::Table::row_length(size_t row)
{
    return rows[row].data.size();
}

bool vp::Table::has_cell(std::size_t row, std::size_t col)
{
    if (row > height()) return false;
    return col < row_length(row);
}

vp::Table::Table()
{

}

std::size_t vp::Table::width() const
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

std::size_t vp::Table::height() const
{
    return rows.size();
}

void vp::Table::do_add_cell(std::string text, std::string classes, CellType type, Mergeable mergeable) {
    if (rows.empty()) { start_new_row(); }
    std::size_t row = height()-1;
    std::size_t col = row_length(row);
    rows.back().data.emplace_back(text, row, col, type, std::move(classes), mergeable);
}

void vp::Table::add_cell(std::string text, std::string classes)
{
    do_add_cell(std::move(text), std::move(classes), CellType::Normal, Mergeable::Yes);
}

void vp::Table::add_unmergeable_cell(std::string text, std::string classes)
{
    do_add_cell(std::move(text), std::move(classes), CellType::Normal, Mergeable::No);
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
    do_add_cell(std::move(text), std::move(classes), CellType::Normal, Mergeable::Yes);
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

bool vp::Table::mergeable_cells(const vp::Table::Cell & c1, const vp::Table::Cell & c2) {
    if (c1.mergeable == Mergeable::No || c2.mergeable == Mergeable::No) return false;
    if (c1.type != c2.type) return false;
    return c1.text == c2.text;
}

void vp::Table::merge_cells_into_rowspans()
{
    if (height() < 2) return;
    std::size_t our_width = width();
    for (size_t col=0; col < our_width; ++col) {
        std::size_t span_size = 1;
        for (size_t row=height()-1; row > 0; --row) {
            if (!has_cell(row, col)) continue;
            auto & cell = at(row, col);
            if (has_cell(row-1, col) && mergeable_cells(cell, at(row-1, col))) {
                ++span_size;
                cell.rowspan = 0;
            } else {
                cell.rowspan = span_size;
                span_size = 1;
            }
        }
        if (has_cell(0, col)) {
            at(0, col).rowspan = span_size;
        }
    }
}

void vp::Table::iterate(vp::Table::CallBack &it) const
{
    for (auto & row : rows) {
        it.row_begin(row.classes);
        for (auto & cell : row.data) {
            if (cell.rowspan == 0) continue;
            if (cell.type == CellType::Header) {
                it.header_cell(cell.text, cell.classes, cell.rowspan);
            } else {
                it.cell(cell.text, cell.classes, cell.rowspan);
            }
        }
        it.row_end();
    }
}

vp::Table::Cell &vp::Table::Iterator::operator*() {
    return table.at(row, col);
}

void vp::Table::Iterator::operator++() {
    if (col+1 < table.row_length(row)) {
        ++col;
    } else {
        col = 0;
        // switch to next row, but skip empty rows
        do {
            ++row;
        } while (row < table.height() && table.row_length(row) == 0);
    }
}

bool vp::Table::Iterator::operator!=(const vp::Table::Iterator &other)
{
    return (row != other.row || col != other.col);
}
