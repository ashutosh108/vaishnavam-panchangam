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
    rows.back().data.emplace_back(std::move(text), row, col, type, std::move(classes), mergeable);
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
    do_add_cell(std::move(text), std::move(classes), CellType::Header, Mergeable::Yes);
}

vp::Table::Cell &vp::Table::at(std::size_t row, std::size_t col)
{
    return rows.at(row).data.at(col);
}

const vp::Table::Cell &vp::Table::at(std::size_t row, std::size_t col) const
{
    return rows.at(row).data.at(col);
}

void vp::Table::start_new_row(std::string classes)
{
    rows.emplace_back(std::move(classes));
}

bool vp::Table::mergeable_cells(const vp::Table::Cell & c1, const vp::Table::Cell & c2) {
    if (c1.mergeable == Mergeable::No || c2.mergeable == Mergeable::No) return false;
    if (c1.type != c2.type) return false;
    return c1.text == c2.text;
}

// make sure we add spans as evenly as possible
void vp::Table::add_row_span(Cell & cell, std::size_t overall_span_size) {
    constexpr std::size_t max_span_size = 12;
    if (overall_span_size < max_span_size) {
        cell.rowspan = overall_span_size;
        return;
    }
    std::size_t num_sections = (overall_span_size + max_span_size - 1) / max_span_size; // rounding up
    std::size_t section_size = (overall_span_size + num_sections - 1) / num_sections; // rounding up
    std::size_t first_row_after_span = cell.row + overall_span_size;
    for (std::size_t row=cell.row; row < first_row_after_span; row += section_size) {
        auto & curr_cell = at(row, cell.col);
        std::size_t span_size = std::min(section_size, first_row_after_span - row);
        curr_cell.rowspan = span_size;

        // merge-to-top all cell runs except the first one
        if (row != cell.row) {
            curr_cell.add_classes("merge-to-top");
        }
        // merge-to-bottom all cell runs except the last one
        if (row + section_size < first_row_after_span) {
            curr_cell.add_classes("merge-to-bottom");
        }
    }
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
                add_row_span(cell, span_size);
                span_size = 1;
            }
        }
        if (has_cell(0, col)) {
            add_row_span(at(0, col), span_size);
        }
    }
}

void vp::Table::add_col_span(Cell & cell, std::size_t span_size) {
    cell.colspan = span_size;
}

void vp::Table::merge_cells_into_colspans()
{
    for (std::size_t row = 0; row < height(); ++row) {
        auto our_row_length = row_length(row);
        if (our_row_length < 2) continue;
        std::size_t span_size = 1;
        for (std::size_t col = our_row_length-1; col > 0; --col) {
            auto & cell = at(row, col);
            if (mergeable_cells(cell, at(row, col-1))) {
                ++span_size;
                cell.colspan = 0;
            } else {
                add_col_span(cell, span_size);
                span_size = 1;
            }
        }
        add_col_span(at(row, 0), span_size);
    }
}

void vp::Table::set_column_widths(std::vector<double> _col_widths)
{
    col_widths_ = _col_widths;
}

const std::vector<double> & vp::Table::column_widths() const
{
    return col_widths_;
}

void vp::Table::add_even_odd_classes_for_col(size_t col, StartFrom start_from)
{
    bool odd = (start_from == StartFrom::Odd);
    vp::Table::Cell * prev_cell{};
    for (size_t row=0; row < height(); ++row) {
        auto & cell = at(row, col);
        // do not switch between odd/even for multiple runs of rowspanning cell with the same text
        if (prev_cell && prev_cell->text != cell.text) {
            odd = !odd;
        }
        if (cell.rowspan >= 1) {
            cell.add_classes(odd ? "odd" : "even");
        }
        prev_cell = &cell;
    }
}

void vp::Table::iterate(vp::Table::CallBack &it) const
{
    for (auto & row : rows) {
        it.row_begin(row.classes);
        for (auto & cell : row.data) {
            if (cell.rowspan == 0 || cell.colspan == 0) continue;
            if (cell.type == CellType::Header) {
                it.header_cell(cell);
            } else {
                it.cell(cell);
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
