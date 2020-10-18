#ifndef VP_TABLE_H
#define VP_TABLE_H

#include <cstdint>
#include <string>
#include <vector>

namespace vp {

class Table
{
public:
    enum class CellType : int8_t { Normal, Header };
    enum class Mergeable : int8_t { No, Yes };
    enum class StartFrom : int8_t { Odd, Even };
    struct Cell {
        std::string text;
        CellType type;
        std::string classes;
        std::string title;
        std::size_t row = 0;
        std::size_t col = 0;
        std::size_t rowspan = 1;
        std::size_t colspan = 1;
        Mergeable mergeable;
        Cell(std::string _text, std::size_t _row, std::size_t _col, CellType _type=CellType::Normal, std::string _classes="", Mergeable _mergeable=Mergeable::Yes) :
              text(_text), type(_type), classes(_classes), row{_row}, col{_col}, mergeable{_mergeable} {}
        void add_classes(std::string new_classes);
        Cell & set_title(std::string new_title);
    };
    struct Row {
        std::vector<Cell> data;
        std::string classes;
        Row(std::string classes_=""):classes(classes_){}
        bool has_class(std::string some_class) const;
    };

private:
    std::vector<Row> rows;
    std::vector<double> col_widths_;
    template<typename Callable>
    void iterate_over_col(std::size_t col, Callable callable) {
        for (std::size_t row=0; row < height(); ++row) {
            if (rows[row].data.size() >= col) {
                callable(row, rows[row].data[col]);
            }
        }
    }
    std::size_t row_length(std::size_t row) const;
    Cell & do_add_cell(std::string text, std::string classes, CellType type, Mergeable mergeable);
    static bool mergeable_cells(const vp::Table::Cell &c1, const vp::Table::Cell &c2);
    void add_row_span(Cell &cell, std::size_t span_size);
    void add_col_span(Cell &cell, std::size_t span_size);

public:
    struct Iterator {
        Table & table;
        std::size_t row;
        std::size_t col;
        Iterator(Table & _table, std::size_t _row, std::size_t _col) : table{_table}, row{_row}, col{_col} {}
        Cell & operator*();
        void operator++();
        bool operator!=(const Iterator &other);
    };
    std::size_t width() const;
    std::size_t height() const;
    Cell & add_cell(std::string text, std::string classes="");
    Cell & add_unmergeable_cell(std::string text, std::string classes="");
    Cell & add_cell(std::string_view text, std::string classes="");
    Cell & add_cell(const char * const text, std::string classes="");
    Cell & add_header_cell(std::string text, std::string classes="");
    Cell & at(size_t row, size_t col);
    const Cell & at(size_t row, size_t col) const;
    void start_new_row(std::string classes="");
    void merge_cells_into_rowspans();
    void merge_cells_into_colspans();
    void set_column_widths(std::vector<double> _col_widths);
    const std::vector<double> & column_widths() const;
    void add_even_odd_classes_for_col(std::size_t col, StartFrom start_from = StartFrom::Odd);
    const Row & row(std::size_t row_num) const;
    bool has_cell(std::size_t row, std::size_t col) const;

    struct CallBack {
        virtual ~CallBack() = default;
        virtual void row_begin(std::string_view classes="") = 0;
        virtual void row_end() = 0;
        virtual void cell(const Cell & cell) = 0;
        virtual void header_cell(const Cell & cell) = 0;
    };

    void iterate(CallBack & it) const;

    Iterator begin() {
        return Iterator{*this, 0, 0};
    }

    Iterator end() {
        return Iterator{*this, height(), 0};
    }
};

} // namespace vp

#endif // VP_TABLE_H
