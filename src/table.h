#ifndef VP_TABLE_H
#define VP_TABLE_H

#include <string>
#include <vector>

namespace vp {

class Table
{
private:
    struct Cell {
        std::string text;
        bool is_header;
        std::string classes;
        std::size_t row = 0;
        std::size_t col = 0;
        std::size_t rowspan = 1;
        Cell(std::string _text, std::size_t _row, std::size_t _col, bool _is_header=false, std::string _classes="") :
              text(_text), is_header(_is_header), classes(_classes), row{_row}, col{_col} {}
    };
    struct Row {
        std::vector<Cell> data;
        std::string classes;
        Row(std::string classes_=""):classes(classes_){}
    };

    std::vector<Row> rows;
    template<typename Callable>
    void iterate_over_col(std::size_t col, Callable callable) {
        for (std::size_t row=0; row < height(); ++row) {
            if (rows[row].data.size() >= col) {
                callable(row, rows[row].data[col]);
            }
        }
    }
    std::size_t row_length(std::size_t row);
    bool has_cell(std::size_t row, std::size_t col);
    void do_add_cell(std::string text, std::string classes, bool is_header);

public:
    struct Iterator {
        Table & table;
        std::size_t row;
        std::size_t col;
        Iterator(Table & _table, std::size_t _row, std::size_t _col) : table{_table}, row{_row}, col{_col} {};
        Cell & operator*();
        void operator++();
        bool operator!=(const Iterator &other);
    };
    Table();
    std::size_t width() const;
    std::size_t height() const;
    void add_cell(std::string text, std::string classes="");
    void add_cell(std::string_view text, std::string classes="");
    void add_cell(const char * const text, std::string classes="");
    void add_header_cell(std::string text, std::string classes="");
    Cell & at(int row, int col);
    const Cell & at(int row, int col) const;
    void start_new_row(std::string classes="");
    void merge_cells_into_rowspans();

    struct CallBack {
        virtual void row_begin(std::string_view classes="") = 0;
        virtual void row_end() = 0;
        virtual void cell(std::string_view text, std::string_view classes, int rowspan) = 0;
        virtual void header_cell(std::string_view text, std::string_view classes, int rowspan) = 0;
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
