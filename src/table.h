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
        Cell(std::string _text, bool _is_header=false) : text(_text), is_header(_is_header){}
    };
    using Row=std::vector<Cell>;

    std::vector<Row> rows;

public:
    Table();
    int width() const;
    int height() const;
    void add_cell(std::string text);
    void add_cell(std::string_view text);
    void add_cell(const char * const text);
    void add_header_cell(std::string text);
    Cell & at(int row, int col);
    const Cell & at(int row, int col) const;
    void start_new_row();

    struct CallBack {
        virtual void row_begin() = 0;
        virtual void row_end() = 0;
        virtual void cell(std::string_view text) = 0;
        virtual void header_cell(std::string_view text) = 0;
    };

    void iterate(CallBack & it) const;
};

} // namespace vp

#endif // VP_TABLE_H
